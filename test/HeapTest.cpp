#include "MemoryManager/Heap.h"
#include "gtest/gtest.h"

namespace {

	TEST(Heap, API) {
		Heap heap(32);

		EXPECT_EQ(heap.h_size, 32);
		EXPECT_EQ(heap.size(), heap.h_size);

		heap[0] = 10;
		EXPECT_EQ(heap[0], 10);
		EXPECT_EQ(heap.storage[0], 10);

		auto p = (Word*)&heap[4];
		auto v = heap.asVirtualAddress(p);
		EXPECT_EQ(v, 4);

		auto pw = heap.asWordPointer(v);
		EXPECT_EQ(pw, p);

		*pw = 100;
		EXPECT_EQ(*p, 100);

		*heap.asBytePointer(0) = 0xFF;
		*heap.asBytePointer(1) = 0xEE;
		*heap.asBytePointer(2) = 0xDD;
		*heap.asBytePointer(3) = 0xCC;

		EXPECT_EQ(*heap.asWordPointer(0), 0xCCDDEEFF);

		heap.reset();
		EXPECT_EQ(*heap.asWordPointer(0), 0x00000000);
	}

}  // namespace