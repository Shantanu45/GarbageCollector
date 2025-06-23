#pragma once

#include <stdint.h>
#include <memory>
#include <set>

struct HeapStats
{
	uint32_t size;

	std::set<uint32_t> usedLocations;

	void MarkUsed(uint32_t from, uint32_t size, bool Unmark = false)
	{
		for (size_t i = from; i < from + size; i++)
		{
			if (Unmark)
			{
				usedLocations.erase(i);
				continue;
			}
			usedLocations.emplace(i);
		}
	}

	void Reset()
	{
		usedLocations.clear();
	}
};