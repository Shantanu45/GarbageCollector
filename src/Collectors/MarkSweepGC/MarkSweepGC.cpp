#include "MarkSweepGC.h"
#include "../../MemoryManager/ObjectHeader.h"

#include <iostream>

std::shared_ptr<GCStats> MarkSweepGC::collect() {
	_resetStats();
	mark();
	sweep();
	return stats;
}

void MarkSweepGC::mark() {
	std::vector<Word> worklist = getRoots();

	while (!worklist.empty()) {
		Word v = worklist.back();
		worklist.pop_back();
		ObjectHeader* header = allocator->getHeader(v);

		if (header->mark == 0)
		{
			header->mark = 1;
			stats->alive++;

			for (const auto& p: allocator->getPointers(v))
			{
				worklist.push_back(p->decode());
			}
		}
	}

}

void MarkSweepGC::sweep()
{
	auto scan = 0 + sizeof(ObjectHeader);

	while (scan < allocator->heap->size()) {
		auto header = allocator->getHeader(scan);

		// Alive object, reset the mark bit for future collection cycles.
		if (header->mark == 1) {
			header->mark = 0;
		}
		else {
			// Garbage, reclaim.
			allocator->free(scan);
			stats->reclaimed++;
		}

		// Move to the next block.
		scan += header->size + sizeof(ObjectHeader);
	}
}
