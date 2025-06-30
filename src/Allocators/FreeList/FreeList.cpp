#include "FreeList.h"
#include "../../Utils/number-util.h"


Value FreeListAllocator::allocate(uint32_t n, std::string name) {
    n = align<uint32_t>(n);

    for (const auto& free : freeList) {
        auto header = (ObjectHeader*)(heap->asWordPointer(free));
        auto size = header->size;

        // Too small block, move further.
        if (size < n) {
            continue;
        }

        // Found block of a needed size:
        freeList.remove(free);

        Word* rawPayload = ((Word*)header) + 1;                      // pointer to the data, next to header data.
        auto payload = heap->asVirtualAddress(rawPayload);          // position of payload of current object

        // See if we can split the larger block, reserving at least
        // one word with a header.
        auto canSplit = (size >= n + (sizeof(Word) * 2));           // (sizeof(Word) * 2) acts as a minimum threshold for the remaining part of the block after the allocation. It ensures that if the block is split, the newly created free block is still a usable minimum size. A usable free block must have at least an ObjectHeader and a minimum of one Word of payload. Given that ObjectHeader has a uint16_t forward, uint8_t size, and a union (e.g., bool mark), its size is likely sizeof(Word) (4 bytes) due to alignment and internal packing. Therefore, (sizeof(Word) * 2) would ensure enough space for the new ObjectHeader (sizeof(Word)) plus at least one Word for the payload of the new free block (sizeof(Word)).

        if (canSplit) {
            // This block becomes of size `n`.
            header->size = n;

            // Split the new block.
            auto nextHeaderP = payload + n;
            auto nextSize = (uint8_t)(size - n - sizeof(ObjectHeader));
            *heap->asWordPointer(nextHeaderP) = ObjectHeader{ .size = nextSize };
            freeList.push_back(nextHeaderP);
        }

        heapStats->MarkUsed(payload - sizeof(ObjectHeader), header->size + sizeof(ObjectHeader), name);

        // Update total object count.
        _objectCount++;

        if (_unTouchedSpace < heap->size())
        {
            _unTouchedSpace = (Word)(payload + n + sizeof(ObjectHeader));
        }

        return Value::Pointer(payload);
    }

    return Value::Pointer(nullptr);
}

Value FreeListAllocator::allocateWithData(uint32_t n, Word* data, std::string name) {
    n = align<uint32_t>(n);

    for (const auto& free : freeList) {
        auto header = (ObjectHeader*)(heap->asWordPointer(free));
        auto size = header->size;

        // Too small block, move further.
        if (size < n) {
            continue;
        }

        // Found block of a needed size:
        freeList.remove(free);

        Word* rawPayload = ((Word*)header) + 1;                      // pointer to the data, next to header data.
        auto payload = heap->asVirtualAddress(rawPayload);          // position of payload of current object

        // See if we can split the larger block, reserving at least
        // one word with a header.
        auto canSplit = (size >= n + (sizeof(Word) * 2));           // (sizeof(Word) * 2) acts as a minimum threshold for the remaining part of the block after the allocation. It ensures that if the block is split, the newly created free block is still a usable minimum size. A usable free block must have at least an ObjectHeader and a minimum of one Word of payload. Given that ObjectHeader has a uint16_t forward, uint8_t size, and a union (e.g., bool mark), its size is likely sizeof(Word) (4 bytes) due to alignment and internal packing. Therefore, (sizeof(Word) * 2) would ensure enough space for the new ObjectHeader (sizeof(Word)) plus at least one Word for the payload of the new free block (sizeof(Word)).

        if (canSplit) {
            // This block becomes of size `n`.
            header->size = n;

            // Split the new block.
            auto nextHeaderP = payload + n;
            auto nextSize = (uint8_t)(size - n - sizeof(ObjectHeader));
            *heap->asWordPointer(nextHeaderP) = ObjectHeader{ .size = nextSize };
            freeList.push_back(nextHeaderP);
        }

        memcpy(rawPayload, data, n);

        heapStats->MarkUsed(payload - sizeof(ObjectHeader), header->size + sizeof(ObjectHeader), name);

        // Update total object count.
        _objectCount++;

        if (_unTouchedSpace < heap->size())
        {
            _unTouchedSpace = (Word)(payload + n + sizeof(ObjectHeader));
        }

        return Value::Pointer(payload);
    }

    return Value::Pointer(nullptr);
}

void FreeListAllocator::free(Word address) {
    auto header = getHeader(address);
    header->mark = 2;

    ptrdiff_t offset = reinterpret_cast<uint8_t*>(header) - heap->asBytePointer(0);
    freeList.push_back(static_cast<uint32_t>(offset));

    auto ptr = heap->asBytePointer(address);

    heapStats->MarkUnUsed(address - sizeof(ObjectHeader));

    // Reset the block to 0. NOT DOING THIS BECAUSE GC WILL TAKE CARE OF IT.
    //memset(heap->asBytePointer(address), 0x0, header->size);

    // Update total object count. Ignoring never allocated space.
    if ((address + header->size) <= _unTouchedSpace)
    {
        _objectCount--;
    }
    _updateFreeStat();
}

void FreeListAllocator::relocate(Word to, Word from, uint32_t size)
{
    std::string name = heapStats->usedLocations[from].name;
    heapStats->MarkUnUsed(from);
    heapStats->MarkUsed(to, size, name);
    auto moveTo = heap->asWordPointer(to);
    auto moveFrom = heap->asWordPointer(from);
    memcpy(moveTo, moveFrom, size);
}

ObjectHeader* FreeListAllocator::getHeader(Word address) {
    return (ObjectHeader*)(heap->asWordPointer(address) - 1);
}

std::vector<Value*> FreeListAllocator::getPointers(Word address) {
    std::vector<Value*> pointers;

    auto header = getHeader(address);
    auto words = header->size / sizeof(Word);

    while (words-- > 0) {
        auto v = (Value*)heap->asWordPointer(address);
        address += sizeof(Word);
        if (!v->isPointer() || v->isNullPointer()) {
            continue;
        }
        pointers.push_back(v);
    }

    return pointers;
}

uint32_t FreeListAllocator::getObjectCount() { return _objectCount; }

void FreeListAllocator::reset() {
    _resetFirstBlock();
    _resetFreeList();
    _objectCount = 0;
}

void FreeListAllocator::_resetFreeList() {
    freeList.clear();
    freeList.push_back(0);
}

void FreeListAllocator::_resetFirstBlock() {
    *heap->asWordPointer(0) = ObjectHeader{
        .size = (uint8_t)(heap->size() - sizeof(ObjectHeader)),
    };
}

void FreeListAllocator::_setFreeRegion(Word from, Word to)
{
    memset(heap->asBytePointer(from), 0x0, (size_t)(to - from));

    *heap->asWordPointer(from) = ObjectHeader{
        .size = (uint8_t)(to - (from + sizeof(ObjectHeader))),
    };

    // TODO: add test for this. if freeList is represeted correctly when Marked Used/Unused
    freeList.clear();
    freeList.push_back(from);
    //auto header = getHeader(from + sizeof(ObjectHeader));
    for (auto used: heapStats->usedLocations)
    {
        if (used.first >= from && used.first < to)
        {
            heapStats->MarkUnUsed(used.first);
        }
    }
    _updateFreeStat();

}

void FreeListAllocator::_updateFreeStat()
{
    heapStats->freeSpace = 0;
    for (auto f: freeList)
    {
        auto header = getHeader(f + sizeof(ObjectHeader));
        heapStats->freeSpace += header->size + sizeof(ObjectHeader);
    }
}
