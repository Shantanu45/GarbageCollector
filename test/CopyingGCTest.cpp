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

namespace GC_TEST
{
	TEST(Copying, API) {
		EXPECT_EQ(cgc.allocator->heap->size(), 32);
		EXPECT_EQ(cgc.ToAllocator->heap->size(), 32);
		EXPECT_EQ(cgc.FromAllocator->heap->size(), 32);
		//cgc.forward();
	}

	TEST(CopyingGC, Copy)
	{
		//auto p1 = cgc.allocator->allocate(4);

		//cgc.copy();
		//for (size_t i = 0; i < 8; i++)
		//{
		//	EXPECT_EQ(cgc.FromHeap->storage[i], cgc.ToHeap->storage[i]);
		//}
	}
}