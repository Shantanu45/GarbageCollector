#pragma once

#include <stdint.h>
#include <memory>
#include <set>

struct HeapStats
{
	struct unit
	{
		uint32_t from;
		uint32_t size;

		bool operator<(const unit& other) const {
			return from < other.from || (from == other.from && size < other.size);
		}
	};
	uint32_t size;

	std::set<unit> usedLocations;

	void MarkUsed(uint32_t from, uint32_t size, bool Unmark = false)
	{
		if (Unmark)
		{
			usedLocations.erase({ from, size });
			return;
		}
		usedLocations.emplace(from, size);

	}

	void Reset()
	{
		usedLocations.clear();
	}
};