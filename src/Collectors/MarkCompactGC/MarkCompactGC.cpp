#include "../../MemoryManager/ObjectHeader.h"
#include "MarkCompactGC.h"

#include <iostream>

std::shared_ptr<GCStats> MarkCompactGC::collect() {
	_resetStats();
	mark();
	compact();
	return stats;
}

void MarkCompactGC::mark() {
	ScopedGCTimer t(profiler, GCTimerID::Mark);
	MarkAllAlive(getRoots(), allocator, stats);
}

void MarkCompactGC::compact() {
	ScopedGCTimer t(profiler, GCTimerID::Compact);
	_computeLocations();
	_updateReferences();
	_relocate();
}

void MarkCompactGC::_computeLocations() {
	UpdateForwardAddrToFree(allocator, stats);
}

void MarkCompactGC::_updateReferences() {
	// Update all root pointers to their forwarded addresses
	for (Word& root : _roots) {
		ObjectHeader* header = allocator->getHeader(root);
		root = header->forward;
	}

	// Scan the heap and update pointers in all live objects
	uint32_t scan = sizeof(ObjectHeader); // Skip over any heap prologue if needed

	while (scan < allocator->heap->size()) {
		ObjectHeader* header = allocator->getHeader(scan);

		if (header->mark == 1) {
			UpdateChildPtrToForwardAddr(scan, allocator);
		}

		scan += header->size + sizeof(ObjectHeader);
	}
}


void MarkCompactGC::_relocate() {
	uint32_t scan = sizeof(ObjectHeader); // Start scanning after heap prologue
	uint32_t freePointer = 0;

	while (scan < allocator->heap->size()) {
		ObjectHeader* header = allocator->getHeader(scan);

		if (header->mark == 1) {
			Word relocatedAddr = RelocateToForwardAddr(scan - sizeof(ObjectHeader), allocator);
			freePointer = std::max(freePointer, static_cast<uint32_t>(
				relocatedAddr + sizeof(ObjectHeader) + header->size));

			// Reset mark for next GC cycle
			header->mark = 0;
		}

		scan += header->size + sizeof(ObjectHeader);
	}

	allocator->setFreeTailRegion(freePointer);
}

