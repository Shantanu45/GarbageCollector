#pragma once

#include <list>
#include <memory>

#include "../ICollector.h"

#include "../../Value/Value.h"
#include "../../Allocators/IAllocator.h"


#ifdef ENABLED_GC_TEST

#include "gtest/gtest.h"
namespace GC_TEST {
	class MarkCompactGCTest_ComputeLocation_Test;
	class MarkCompactGCTest_UpdateReferences_Test;
	class MarkCompactGCTest_Relocate_Test;
	class MarkCompactGCTest_Mark_Test;
}
#endif

 /**
  * Mark-Compact garbage collector.
  *
  *   - Mark phase: trace, marks reachable objects as alive
  *   - Compact phase: relocates objects in place using Lisp2 algorithm.
  *
  * Collects stats during collection.
  *
  */
class MarkCompactGC : public ICollector {
public:
	MarkCompactGC(const std::shared_ptr<IAllocator>& allocator)
		: ICollector(allocator) {
		stats->name = getCollectorName();
	};

	/**
	 * Main collection cycle.
	 */
	std::shared_ptr<GCStats> collect();

	std::string getCollectorName() { 
		return "Mark Compact"; 
	}


private:

#ifdef ENABLED_GC_TEST
	FRIEND_TEST(GC_TEST::MarkCompactGCTest, ComputeLocation);
	FRIEND_TEST(GC_TEST::MarkCompactGCTest, UpdateReferences);
	FRIEND_TEST(GC_TEST::MarkCompactGCTest, Relocate);
	FRIEND_TEST(GC_TEST::MarkCompactGCTest, Mark);
#endif

	/**
	 * Mark phase. Returns number of live objects.
	 */
	void mark();

	/**
	 * Compact phase. Resets the mark bit, relocates objects to
	 * one side of the heap, doing a defragmentation, using Lisp2 algorithm.
	 */
	void compact();

	/**
	 * Computes new locations for the objects.
	 */
	void _computeLocations();

	/**
	 * Updates child references of the object according
	 * to the new locations.
	 */
	void _updateReferences();

	/**
	 * Relocates the objects to the new locations.
	 */
	void _relocate();
};