#pragma once
#include "../MemoryManager/MemoryManager.h"
#include "spdlog/spdlog.h"
#include <sstream> // for std::ostringstream

inline void printGCStats(std::shared_ptr<MemoryManager> mm)
{
	std::shared_ptr<GCStats> gcStats = mm->getGCStats();
	spdlog::info("    total:{}", gcStats->total);
	spdlog::info("    alive:{}", gcStats->alive);
	spdlog::info("reclaimed:{}", gcStats->reclaimed);				// will include every unreachalbe block including the empty space at then end of heap...even if we didn't allocate it, its not reachable from the root and will be reclainmed.
}

inline void printHeapStats(std::shared_ptr<MemoryManager> mm)
{
	std::ostringstream out;
	for (size_t i = 0; i < mm->heap->size(); i++)
	{
		if (mm->allocator->heapStats->usedLocations.find(i) != mm->allocator->heapStats->usedLocations.end())
		{
			out << "=";
		}
		else
		{
			out << "-";
		}
	}
	spdlog::info("Heap usage: {}", out.str());
}