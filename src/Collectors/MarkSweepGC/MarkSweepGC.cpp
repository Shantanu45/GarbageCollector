#include "MarkSweepGC.h"
#include "../../MemoryManager/ObjectHeader.h"

#include <iostream>

std::shared_ptr<GCStats> MarkSweepGC::collect() {
	if (_roots.empty())
	{
		_roots.push_back(0 + sizeof(ObjectHeader));
	}
	_resetStats();
	mark();
	sweep();
	return stats;
}

void MarkSweepGC::mark() {
	ScopedGCTimer t(profiler, GCTimerID::Mark);
	MarkAllAlive(getRoots(), allocator, stats);
}

void MarkSweepGC::sweep()
{
	ScopedGCTimer t(profiler, GCTimerID::Sweep);

	Word scan = 0 + sizeof(ObjectHeader);

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
