#include "api.h"
#include "Collectors/MarkCompactGC/MarkCompactGC.h"
#include "MemoryManager/MemoryManager.h"
#include "Allocators/FreeList/FreeList.h"
#include "gtest/gtest.h"

namespace
{

	static auto heap = std::make_shared<Heap>(64);
	static auto allocator = std::make_shared<FreeListAllocator>(heap);
	static MarkCompactGC mcgc(allocator);

	static auto mm = MemoryManager::create<FreeListAllocator, MarkCompactGC, 64>();

	TEST(API, TESTFN)
	{
		GSetActiveMemoryManager_API(mm);
		auto p1 = mm->allocate(4);
		auto p2 = mm->allocate(4);

		mm->collect();
		UpdateStats();

		EXPECT_EQ(GetHeapStats()->totalSize, 64);
		EXPECT_EQ(GetHeapStats()->usedLocations->size, 8);
		EXPECT_EQ(GetHeapStats()->usedLocations->from, 0);


		EXPECT_EQ(GetGCStats()->alive, 1);
		EXPECT_EQ(GetGCStats()->reclaimed, 2);
		EXPECT_EQ(GetGCStats()->total, 2);
	}
}