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

void MarkCompactGC::compact() {
	_computeLocations();
	_updateReferences();
	_relocate();
}

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

void MarkCompactGC::_relocate() {
	auto scan = 0 + sizeof(ObjectHeader);

	uint32_t freePointer = 0;

	while (scan < allocator->heap->size()) {
		auto header = allocator->getHeader(scan);

		// Alive object, reset the mark bit for future collection cycles.
		if (header->mark == 1) {
			auto toptr = header->forward - sizeof(ObjectHeader);
			auto fromptr = scan - sizeof(ObjectHeader);
			allocator->relocate(toptr, fromptr, header->size + sizeof(ObjectHeader));
			freePointer = std::max(freePointer, (uint32_t)(toptr + sizeof(ObjectHeader) + header->size)/*freePointer + scan*/);
			header->mark == 0;
		}

		// Move to the next block.
		scan += header->size + sizeof(ObjectHeader);
	}

	allocator->setFreeRegion(freePointer);
}
