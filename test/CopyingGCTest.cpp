#include "Collectors/CopyingGC/CopyingGC.h"
#include "MemoryManager/MemoryManager.h"
#include "Allocators/FreeList/FreeList.h"
#include "gtest/gtest.h"

#include "spdlog/spdlog.h"

static auto heap = std::make_shared<Heap>(32);
static auto allocator = std::make_shared<FreeListAllocator>(heap);

static auto heapTwo = std::make_shared<Heap>(32);
static auto allocatorTwo = std::make_shared<FreeListAllocator>(heapTwo);
static CopyingGC cgc(allocator, allocatorTwo);

namespace
{
	void reset() {
		heap->reset();
		heapTwo->reset();
		allocator->reset();
		allocatorTwo->reset();
	}

	TEST(Copying, API) {
		EXPECT_EQ(cgc.allocator->heap->size(), 32);
		EXPECT_EQ(cgc.ToAllocator->heap->size(), 32);
		EXPECT_EQ(cgc.FromAllocator->heap->size(), 32);
		//cgc.forward();
	}

	TEST(CopyingGC, Copy)
	{
		reset();

		auto p1 = cgc.FromAllocator->allocate(4);

		auto p2 = cgc.FromAllocator->allocate(8);

		*cgc.FromAllocator->heap->asWordPointer(p1) = p2;

		*cgc.FromAllocator->heap->asWordPointer(p2) = Value::Pointer(p1);

		cgc.copy();

		EXPECT_EQ(cgc.virtualAddressRelativeToToHeap(cgc.FromAllocator->getHeader(p1)->forward), 4);
		EXPECT_EQ(cgc.virtualAddressRelativeToToHeap(cgc.FromAllocator->getHeader(p2)->forward), 12);

		EXPECT_TRUE(cgc.isForwardPointingToSwapHeap(p2));

		EXPECT_EQ(cgc.ToAllocator->getHeader(4)->size, cgc.FromAllocator->getHeader(4)->size);
		EXPECT_EQ(cgc.ToAllocator->getHeader(4)->size, 4);
		EXPECT_EQ(cgc.ToAllocator->getHeader(12)->size, cgc.FromAllocator->getHeader(12)->size);
		EXPECT_EQ(cgc.ToAllocator->getHeader(12)->size, 8);


		EXPECT_EQ(*cgc.ToAllocator->heap->asWordPointer(4), 12);
		EXPECT_EQ(*cgc.ToAllocator->heap->asWordPointer(12), 4);
		
		//EXPECT_TRUE(memcmp((const void*)(allocator->heap->storage + 4), (const void*)(allocatorTwo->heap->storage + 4), 4) == 0);		// data should still be the same. Header won't be the same though as forward was change duirng copy()

		EXPECT_TRUE(memcmp((const void*)(cgc.FromAllocator->heap->storage + 12), (const void*)(cgc.ToAllocator->heap->storage + 12), 4) == 0);

	}

	TEST(CopyingGC, Collect)
	{
		reset();

		auto p1 = cgc.allocator->allocate(4);

		auto p2 = cgc.allocator->allocate(8);

		*allocator->heap->asWordPointer(p1) = p2;

		*allocator->heap->asWordPointer(p2) = Value::Pointer(p1);

		cgc.collect();

		//after swap
		EXPECT_EQ(cgc.FromAllocator->getHeader(4)->size, 4);
		EXPECT_EQ(cgc.FromAllocator->getHeader(12)->size, 8);

		EXPECT_EQ(*cgc.FromAllocator->heap->asWordPointer(4), 12);
		EXPECT_EQ(*cgc.FromAllocator->heap->asWordPointer(12), 4);
		//EXPECT_EQ(cgc.virtualAddressRelativeToToHeap(allocator->getHeader(p2)->forward), 12);

	}
}