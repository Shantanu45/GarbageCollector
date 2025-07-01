#pragma once

#include <stdint.h>
#include <memory>
#include <set>
#include <string>
#include "../Utils/timer-util.h"

/**
 * Stats for the collection cycle.
 */
struct GCStats {
	/**
	 * The snapshot of the total objects on the heap before collection.
	 */
	uint32_t total;

	/**
	 * Number of alive (reachable) objects.
	 */
	uint32_t alive;

	/**
	 * Number of reclaimed objects.
	 */
	uint32_t reclaimed;

	std::string name;
	std::string allocatorName;

	std::array<double, static_cast<size_t>(GCTimerID::Count)>* durations;
};      