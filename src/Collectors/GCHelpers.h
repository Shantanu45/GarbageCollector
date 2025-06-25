#pragma once

#include "../MemoryManager/ObjectHeader.h"
#include "../Allocators/IAllocator.h"

inline void MarkAllAlive(std::vector<Word> roots, std::shared_ptr<IAllocator> allocator, std::shared_ptr<GCStats> stats)
{
	std::vector<Word> worklist = roots;

	while (!worklist.empty()) {
		Word v = worklist.back();
		worklist.pop_back();
		ObjectHeader* header = allocator->getHeader(v);

		if (header->mark == 0)
		{
			header->mark = 1;
			stats->alive++;

			for (const auto& p : allocator->getPointers(v))
			{
				worklist.push_back(p->decode());
			}
		}
	}
}

inline Word RelocatreToAddr(Word src, Word dst, std::shared_ptr<IAllocator> allocator)
{
	auto header = (ObjectHeader*)allocator->heap->asWordPointer(src);
	auto toptr = dst;
	auto fromptr = src;
	allocator->relocate(toptr, fromptr, header->size + sizeof(ObjectHeader));
	return toptr;
}

// src: virtual address of ObjectHeader
// moves ObjectHeader + data
inline Word RelocatreToForwardAddr(Word src, std::shared_ptr<IAllocator> allocator)
{
	auto header = (ObjectHeader*)allocator->heap->asWordPointer(src);
	return RelocatreToAddr(src, header->forward - sizeof(ObjectHeader), allocator);
}

// return actual address of destination
inline Word CopyToNewHeap(Word src, std::shared_ptr<IAllocator> srcAllocator, std::shared_ptr<IAllocator> dstAllocator)
{
	auto header = (ObjectHeader*)srcAllocator->heap->asWordPointer(src);
	Word* data = ((Word*)header) + 1;
	auto dstData = dstAllocator->allocateWithData(header->size, data);
	return dstData;
}


inline void UpdateChildPtrToForwardAddr(Word src, std::shared_ptr<IAllocator> allocator)
{
	for (const auto& p : allocator->getPointers(src))
	{
		ObjectHeader* childHeader = allocator->getHeader((Word)p->decode());
		*p = childHeader->forward;
	}
}

inline void UpdateForwardAddrToFree(std::shared_ptr<IAllocator> allocator, std::shared_ptr<GCStats> stats)
{
	auto scan = 0 + sizeof(ObjectHeader);
	auto free = scan;

	while (scan < allocator->heap->size()) {
		auto header = allocator->getHeader(scan);

		// Alive object, reset the mark bit for future collection cycles.
		if (header->mark == 1) {
			//header->mark = 0;
			header->forward = free;
			free += header->size + sizeof(ObjectHeader);
		}
		else {
			stats->reclaimed++;
		}

		// Move to the next block.
		scan += header->size + sizeof(ObjectHeader);
	}
}

inline void UpdateForwardAddr(Word src, Word forward, std::shared_ptr<IAllocator> allocator)
{
	auto header = allocator->getHeader(src);
	header->forward = forward;
}

inline void ReplaceHeaderWithRawPtr(Word src, std::shared_ptr<IAllocator> allocator, Word* forward)
{
	Word* rawSrc = allocator->heap->asWordPointer(src);
	memset(rawSrc + sizeof(ObjectHeader), 0x0, allocator->getHeader(src)->size);
	*rawSrc = reinterpret_cast<Word>(forward);
}

//inline bool isVirtualAddressInOriginalHeap(Word src, std::shared_ptr<IAllocator> allocatorOne) {
//	auto header = allocatorOne->getHeader(src);
//	auto factor = header->forward / allocatorOne->heap->size();
//	if (factor < 1 && factor >= 0)
//	{
//		return true;
//	}
//	return false;
//}
//
//inline bool isVirtualAddressInSwapHeap(Word src, std::shared_ptr<IAllocator> allocatorOne) {
//	auto header = allocatorOne->getHeader(src);
//	auto factor = src / allocatorOne->heap->size();
//	if (factor >= 1 && factor < 2)
//	{
//		return true;
//	}
//	return false;
//}

// takes src of allocatorOne
//inline bool isForwardPointingToSwapHeap(Word src, std::shared_ptr<IAllocator> allocatorOne)
//{
//	auto header = allocatorOne->getHeader(src);
//	return isVirtualAddressInSwapHeap(header->forward, allocatorOne);
//}


