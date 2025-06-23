#pragma once

#include <stdint.h>
#include <memory>
#include <set>
#include "../MemoryManager/MemoryManager.h"

/**
 * Stats for the heap.
 */
struct HeapStats {

	uint32_t totalSize;

	/*
	* Free memory
	*/
	std::set<uint32_t> free;

	/*
	* Used memory
	*/
	std::set<uint32_t> usedLocations;
};