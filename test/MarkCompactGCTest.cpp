#include "Collectors/MarkCompactGC/MarkCompactGC.h"
#include "MemoryManager/MemoryManager.h"
#include "Allocators/FreeList/FreeList.h"
#include "gtest/gtest.h"

namespace {

	static auto heap = std::make_shared<Heap>(64);
	static auto allocator = std::make_shared<FreeListAllocator>(heap);
	static MarkCompactGC mcgc(allocator);

	void reset() {
		heap->reset();
		allocator->reset();
	}

	TEST(MarkCompactGCTest, API) {
		EXPECT_EQ(mcgc.allocator->heap->size(), 64);
	}

	TEST(MarkCompactGCTest, Mark) {
		reset();

		auto p1 = mcgc.allocator->allocate(4);
		auto p2 = mcgc.allocator->allocate(4);

		*mcgc.allocator->heap->asWordPointer(p1) = p2;

		mcgc.mark();

		auto headerP1 = mcgc.allocator->getHeader(p1);
		auto headerP2 = mcgc.allocator->getHeader(p2);

		EXPECT_TRUE(headerP1->mark);
		EXPECT_TRUE(headerP2->mark);

	}

	TEST(MarkCompactGCTest, ComputeLocation)
	{
		reset();

		auto p1 = mcgc.allocator->allocate(4);

		auto p2 = mcgc.allocator->allocate(4);

		auto p3 = mcgc.allocator->allocate(4);

		*mcgc.allocator->heap->asWordPointer(p1) = p3;

		auto p4 = mcgc.allocator->allocate(4);

		auto p5 = mcgc.allocator->allocate(4);

		*mcgc.allocator->heap->asWordPointer(p3) = p5;

		mcgc.allocator->allocate(4);

		mcgc.mark();
		mcgc._computeLocations();

		auto headerP1 = mcgc.allocator->getHeader(p1);
		auto headerP3 = mcgc.allocator->getHeader(p3);
		auto headerP5 = mcgc.allocator->getHeader(p5);

		EXPECT_EQ(headerP1->forward, p1.decode());
		EXPECT_EQ(headerP3->forward, p2.decode());
		EXPECT_EQ(headerP5->forward, p3.decode());

	}

	TEST(MarkCompactGCTest, UpdateReferences)
	{
		reset();

		auto p1 = mcgc.allocator->allocate(4);

		auto p2 = mcgc.allocator->allocate(4);

		auto p3 = mcgc.allocator->allocate(4);

		*mcgc.allocator->heap->asWordPointer(p1) = p3;

		auto p4 = mcgc.allocator->allocate(4);

		auto p5 = mcgc.allocator->allocate(4);

		*mcgc.allocator->heap->asWordPointer(p3) = p5;

		mcgc.allocator->allocate(4);

		mcgc.mark();
		mcgc._computeLocations();
		mcgc._updateReferences();

		auto childP1 = *mcgc.allocator->getPointers(p1)[0];

		EXPECT_EQ(childP1.decode(), p2.decode());

		auto childP3 = *mcgc.allocator->getPointers(p3)[0];

		EXPECT_EQ(childP3.decode(), p3.decode());

	}

	TEST(MarkCompactGCTest, Relocate)
	{
		reset();

		auto p1 = mcgc.allocator->allocate(4);

		auto p2 = mcgc.allocator->allocate(4);

		auto p3 = mcgc.allocator->allocate(4);

		*mcgc.allocator->heap->asWordPointer(p1) = p3;

		auto p4 = mcgc.allocator->allocate(4);

		auto p5 = mcgc.allocator->allocate(4);

		*mcgc.allocator->heap->asWordPointer(p3) = p5;

		mcgc.allocator->allocate(4);

		mcgc.mark();
		mcgc._computeLocations();
		mcgc._updateReferences();
		mcgc._relocate();

		auto headerP2 = mcgc.allocator->getHeader(p2);

		EXPECT_TRUE(headerP2->mark);

		auto childP2 = *mcgc.allocator->getPointers(p2)[0];

		EXPECT_EQ(childP2.decode(), p3);

		auto headerP3 = mcgc.allocator->getHeader(p3);

		EXPECT_TRUE(headerP3->mark);

		auto headerP4 = mcgc.allocator->getHeader(p4);

		EXPECT_FALSE(headerP4->mark);
	}
}