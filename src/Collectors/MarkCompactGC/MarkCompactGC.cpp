#include "../../MemoryManager/ObjectHeader.h"
#include "MarkCompactGC.h"

#include <iostream>

 /**
  * Main collection cycle.
  */
std::shared_ptr<GCStats> MarkCompactGC::collect() {
	_resetStats();
	mark();
	compact();
	return stats;
}

/**
 * Mark phase.
 */
void MarkCompactGC::mark() {
	auto worklist = getRoots();

	while (!worklist.empty()) {
		auto v = worklist.back();
		worklist.pop_back();
		auto header = allocator->getHeader(v);

		// Mark the object if it's not marked yet, and move to the child pointers.
		if (header->mark == 0) {
			header->mark = 1;
			stats->alive++;
			for (const auto& p : allocator->getPointers(v)) {
				worklist.push_back(p->decode());
			}
		}
	}
}

/**
 * Compact phase using Lisp2 algorithm.
 */
void MarkCompactGC::compact() {
	_computeLocations();
	_updateReferences();
	_relocate();
}

/**
 * Computes new locations for the objects.
 */
void MarkCompactGC::_computeLocations() {
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

/**
 * Updates child references of the object according
 * to the new locations.
 */
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


		for (const auto& p : allocator->getPointers(scan))
		{
			ObjectHeader* childHeader = allocator->getHeader((Word)p->decode());
			*p = childHeader->forward;
		}

		scan += header->size + sizeof(ObjectHeader);
	}
}

/**
 * Relocates the objects to the new locations.
 */
void MarkCompactGC::_relocate() {
	auto scan = 0 + sizeof(ObjectHeader);

	auto freePointer = scan;

	while (scan < allocator->heap->size()) {
		auto header = allocator->getHeader(scan);

		// Alive object, reset the mark bit for future collection cycles.
		if (header->mark == 1) {
			auto toptr = header->forward - sizeof(ObjectHeader);
			auto moveTo = allocator->heap->asWordPointer(toptr);
			auto fromptr = scan - sizeof(ObjectHeader);
			auto from = allocator->heap->asWordPointer(fromptr);
			memcpy(moveTo, from, header->size + sizeof(ObjectHeader));
			freePointer = std::max(freePointer, freePointer + scan);
		}

		// Move to the next block.
		scan += header->size + sizeof(ObjectHeader);
	}

	allocator->setFreeRegion(freePointer - sizeof(ObjectHeader));
}
