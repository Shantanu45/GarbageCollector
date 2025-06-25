#include "MemoryManager/Heap.h"
#include "MemoryManager/ObjectHeader.h"
#include "Allocators/FreeList/FreeList.h"
#include "Value/Value.h"
#include "Collectors/GCHelpers.h"

#include "gtest/gtest.h"

namespace {

	static auto heapOne = std::make_shared<Heap>(32);
	static auto allocatorOne = std::make_shared<FreeListAllocator>(heapOne);

	static auto heapTwo = std::make_shared<Heap>(32);
	static auto allocatorTwo = std::make_shared<FreeListAllocator>(heapTwo);


	TEST(GCHelpers, CopyToNewHeap)
	{
		auto p1 = allocatorOne->allocate(4);
		*allocatorOne->heap->asWordPointer(p1) = Value::Number(2);
		CopyToNewHeap(0, allocatorOne, allocatorTwo);
		EXPECT_TRUE(std::memcmp((Word*)allocatorOne->getHeader(p1) + 1, (Word*)allocatorTwo->getHeader(4) + 1, 4) == 0);
		EXPECT_EQ(allocatorOne->getHeader(p1)->size, 4);

		//CopyToNewHeap(0, allocatorOne, 1, allocatorTwo);
		//EXPECT_TRUE(std::memcmp((Word*)allocatorOne->getHeader(p1)+1, (Word*)allocatorTwo->getHeader(4) + 1, 4) == 0);
	}

	TEST(GCHelpers, UpdateForwardAddr)
	{
		auto p1 = allocatorOne->allocate(4);
		auto p2 = allocatorTwo->allocate(4);

		UpdateForwardAddr(p1, allocatorOne->heap->size() + p2, allocatorOne);

		EXPECT_EQ(allocatorOne->getHeader(p1)->forward, allocatorOne->heap->size() + p2);
	}
}