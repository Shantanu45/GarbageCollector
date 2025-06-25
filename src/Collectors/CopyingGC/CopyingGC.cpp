#include "CopyingGC.h"
#include "../../Allocators/FreeList/FreeList.h"

std::shared_ptr<GCStats> CopyingGC::collect() {
	_resetStats();
	copy();
	swap();

	return stats;
}

void CopyingGC::copy()
{
	std::vector<Word> worklist = _roots;

	Word newLoc;
	dstAlloc = 0 + sizeof(ObjectHeader);

	while (!worklist.empty()) {
		Word v = worklist.back();
		worklist.pop_back();
		newLoc = copyBlock(v);
	}
}

Word CopyingGC::copyBlock(Word src)
{
	stats->alive++;
	ObjectHeader* header = FromAllocator->getHeader(src);
	auto newLoc = virtualAddressRelativeToFromHeap(CopyToNewHeap(src - sizeof(ObjectHeader), FromAllocator, dstAlloc - sizeof(ObjectHeader), ToAllocator));
	dstAlloc += sizeof(ObjectHeader) + header->size;

	forward(src, newLoc);
	for (const auto& p : ToAllocator->getPointers(virtualAddressRelativeToToHeap(newLoc)))
	{
		if (isVirtualAddressInFromHeap(*p))
		{
			if (!isForwardPointingToSwapHeap(*p))
			{
				auto movedPtr = copyBlock(p->decode());
				fixPointer(p, virtualAddressRelativeToToHeap(movedPtr));
			}
			else
			{
				auto ptr = virtualAddressRelativeToToHeap(FromAllocator->getHeader((Word)p->decode())->forward);
				ToAllocator->getHeader(virtualAddressRelativeToToHeap(newLoc))->forward = ptr;
			}
		}
	}
	return newLoc;
}

void CopyingGC::forward(Word src, Word dst)
{
	UpdateForwardAddr(src, dst, FromAllocator);
}

void CopyingGC::fixPointer(Value* ptr, Word src)
{
	*ptr = src;
}

void CopyingGC::swap()
{
	FromAllocator->setFreeRegion(0);
	std::swap(FromAllocator, ToAllocator);
	//allocator = FromAllocator;
	//std::shared_ptr<IAllocator> tempAlloc = FromAllocator;
	//FromAllocator = ToAllocator;
	//ToAllocator = tempAlloc;
}

bool CopyingGC::isVirtualAddressInFromHeap(Word src) {
	auto header = FromAllocator->getHeader(src);
	auto factor = header->forward / FromAllocator->heap->size();
	if (factor < 1 && factor >= 0)
	{
		return true;
	}
	return false;
}

bool CopyingGC::isVirtualAddressInToHeap(Word src) {
	//auto header = FromAllocator->getHeader(src);
	auto factor = src / FromAllocator->heap->size();
	if (factor >= 1 && factor < 2)
	{
		return true;
	}
	return false;
}

uint16_t CopyingGC::virtualAddressRelativeToToHeap(Word src)
{
	return src % FromAllocator->heap->size();
}

uint16_t CopyingGC::virtualAddressRelativeToFromHeap(Word src)
{
	return FromAllocator->heap->size() + src;
}

bool CopyingGC::isForwardPointingToSwapHeap(Word src)
{
	auto header = FromAllocator->getHeader(src);
	return isVirtualAddressInToHeap(header->forward);
}
