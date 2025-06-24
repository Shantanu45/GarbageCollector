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
	MarkAllAlive(getRoots(), allocator, stats);
}

void MarkCompactGC::compact() {
	_computeLocations();
	_updateReferences();
	_relocate();
}

void MarkCompactGC::_computeLocations() {
	UpdateForwardAddrToFree(allocator, stats);
}

void MarkCompactGC::_updateReferences() {
	std::vector<Word> roots = _roots;
	for (auto root : roots)
	{
		_roots.pop_back();

		auto header = allocator->getHeader(root);
		auto newRoot = header->forward;
		_roots.push_back(newRoot);
	}

	auto scan = 0 + sizeof(ObjectHeader);

	while (scan < allocator->heap->size())
	{
		ObjectHeader* header = allocator->getHeader(scan);

		if (header->mark == 1)
		{
			UpdateChildPtrToForwardAddr(scan, allocator);
		}

		scan += header->size + sizeof(ObjectHeader);
	}
}

void MarkCompactGC::_relocate() {
	auto scan = 0 + sizeof(ObjectHeader);

	uint32_t freePointer = 0;

	while (scan < allocator->heap->size()) {
		auto header = allocator->getHeader(scan);

		// Alive object, reset the mark bit for future collection cycles.
		if (header->mark == 1) {
			auto toptr = RelocatreToForwardAddr(scan - sizeof(ObjectHeader), allocator);
			freePointer = std::max(freePointer, (uint32_t)(toptr + sizeof(ObjectHeader) + header->size)/*freePointer + scan*/);
			header->mark == 0;
		}

		// Move to the next block.
		scan += header->size + sizeof(ObjectHeader);
	}

	allocator->setFreeRegion(freePointer);
}
