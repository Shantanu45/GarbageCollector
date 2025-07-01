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
	ScopedGCTimer t(profiler, GCTimerID::Copy);
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

	// Get header and calculate new location in ToHeap
	ObjectHeader* header = FromAllocator->getHeader(src);
	Word newLoc = virtualAddressRelativeToFromHeap(
		CopyToNewHeap(src - sizeof(ObjectHeader), FromAllocator, ToAllocator)
	);

	// Update destination allocation pointer
	dstAlloc += sizeof(ObjectHeader) + header->size;

	// Set forwarding pointer
	forward(src, newLoc);

	// Get pointer list from the new object's location in ToHeap
	auto pointers = ToAllocator->getPointers(virtualAddressRelativeToToHeap(newLoc));

	for (const auto& p : pointers)
	{
		ObjectHeader* targetHeader = FromAllocator->getHeader(*p);

		// If the object is marked deleted, null out the pointer
		if (targetHeader->mark == 2) {
			*p = 0;
			continue;
		}

		// Only process pointers that are in FromHeap
		if (!isVirtualAddressInFromHeap(*p)) {
			continue;
		}

		// If not already forwarded, move the block and fix the pointer
		if (!isForwardPointingToSwapHeap(*p)) {
			Word movedPtr = copyBlock(p->decode());
			fixPointer(p, virtualAddressRelativeToToHeap(movedPtr));
		}
		else {
			// Already forwarded fix the forward reference in the copied object's header
			Word fwdPtr = FromAllocator->getHeader((Word)p->decode())->forward;
			Word relativeFwdPtr = virtualAddressRelativeToToHeap(fwdPtr);

			ToAllocator->getHeader(virtualAddressRelativeToToHeap(newLoc))->forward = relativeFwdPtr;
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
	FromAllocator->setFreeTailRegion(0);
	allocator = ToAllocator;

	std::swap(FromAllocator, ToAllocator);
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
