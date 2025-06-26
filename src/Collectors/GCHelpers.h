#pragma once

#include "../MemoryManager/ObjectHeader.h"
#include "../Allocators/IAllocator.h"

inline void MarkAllAlive(const std::vector<Word>& roots, const std::shared_ptr<IAllocator>& allocator, const std::shared_ptr<GCStats>& stats)
{
	std::vector<Word> worklist = roots;

	while (!worklist.empty()) {
		Word current = worklist.back();
		worklist.pop_back();

		ObjectHeader* header = allocator->getHeader(current);
		if (header->mark == 0) {
			header->mark = 1;
			stats->alive++;

			for (const auto& p : allocator->getPointers(current)) {
				worklist.push_back(p->decode());
			}
		}
	}
}

inline Word RelocateToAddr(Word src, Word dst, const std::shared_ptr<IAllocator>& allocator)
{
	ObjectHeader* header = reinterpret_cast<ObjectHeader*>(allocator->heap->asWordPointer(src));
	allocator->relocate(dst, src, header->size + sizeof(ObjectHeader));
	return dst;
}

// src: virtual address of ObjectHeader
// moves ObjectHeader + data to header->forward
inline Word RelocateToForwardAddr(Word src, const std::shared_ptr<IAllocator>& allocator)
{
	ObjectHeader* header = reinterpret_cast<ObjectHeader*>(allocator->heap->asWordPointer(src));
	return RelocateToAddr(src, header->forward - sizeof(ObjectHeader), allocator);
}

inline Word CopyToNewHeap(Word src, const std::shared_ptr<IAllocator>& srcAllocator, const std::shared_ptr<IAllocator>& dstAllocator)
{
	ObjectHeader* header = reinterpret_cast<ObjectHeader*>(srcAllocator->heap->asWordPointer(src));
	Word* data = reinterpret_cast<Word*>(header + 1);

	std::string name;
	auto find = srcAllocator->heapStats->usedLocations.find(src);
	if (find != srcAllocator->heapStats->usedLocations.end()) {
		name = find->second.name;
	}

	return dstAllocator->allocateWithData(header->size, data, name);
}

inline void UpdateChildPtrToForwardAddr(Word src, const std::shared_ptr<IAllocator>& allocator)
{
	for (const auto& p : allocator->getPointers(src)) {
		ObjectHeader* childHeader = allocator->getHeader(p->decode());
		*p = childHeader->forward;
	}
}

inline void UpdateForwardAddrToFree(const std::shared_ptr<IAllocator>& allocator, const std::shared_ptr<GCStats>& stats)
{
	uint32_t scan = sizeof(ObjectHeader);
	uint32_t free = scan;

	while (scan < allocator->heap->size()) {
		ObjectHeader* header = allocator->getHeader(scan);

		if (header->mark == 1) {
			header->forward = free;
			free += header->size + sizeof(ObjectHeader);
		}
		else {
			stats->reclaimed++;
		}

		scan += header->size + sizeof(ObjectHeader);
	}
}

inline void UpdateForwardAddr(Word src, Word forward, const std::shared_ptr<IAllocator>& allocator)
{
	allocator->getHeader(src)->forward = forward;
}

inline void ReplaceHeaderWithRawPtr(Word src, const std::shared_ptr<IAllocator>& allocator, Word* forward)
{
	Word* rawPtr = allocator->heap->asWordPointer(src);
	ObjectHeader* header = allocator->getHeader(src);

	memset(rawPtr + sizeof(ObjectHeader), 0x0, header->size);
	*rawPtr = reinterpret_cast<Word>(forward);
}
