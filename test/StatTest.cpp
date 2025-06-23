#include "Stats/GCStats.h"
#include "Stats/HeapStats.h"
#include "gtest/gtest.h"

namespace {
	TEST(Stats, GCStats)
	{

	}

	TEST(Stats, HeapStats)
	{
		HeapStats stat = {
			.usedLocations = {1, 2, 4, 8, 4},
		};

		EXPECT_EQ(stat.usedLocations.size(), 4);

		stat.Reset();
		EXPECT_EQ(stat.usedLocations.size(), 0);

		stat.MarkUsed(5, 3);

		std::set<uint32_t> expectedUsed = { 5, 6, 7 };

		EXPECT_EQ(stat.usedLocations.size(), 3);
		EXPECT_EQ(stat.usedLocations, expectedUsed);

		stat.MarkUsed(6, 2, true);
		std::set<uint32_t> expectedAfterUnmark = { 5 };

		EXPECT_EQ(stat.usedLocations.size(), 1);
		EXPECT_EQ(stat.usedLocations, expectedAfterUnmark);

		stat.MarkUsed(5, 1, true);
		EXPECT_EQ(stat.usedLocations.size(), 0);

	}
}