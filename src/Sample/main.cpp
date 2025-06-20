
#include <iostream>
#include <memory>

#include "MemoryManager/MemoryManager.h"
#include "Allocators/FreeList/FreeList.h"
#include "Collectors/MarkSweepGC/MarkSweepGC.h"
#include "Collectors/MarkCompactGC/MarkCompactGC.h"
#include "Value/Value.h"

#define log(title, value) std::cout << title << " " << value << std::endl
//#define log(text) std::cout << text << std::endl

int main()
{
	auto mm = MemoryManager::create<FreeListAllocator, MarkCompactGC, 64>();

	auto p1 = mm->allocate(12);

	mm->writeWord(p1, Value::Number(1));
	mm->writeValue(p1 + 2, Value::Pointer(p1));

	auto p2 = mm->allocate(4);

	mm->writeValue(p2, Value::Number(2));
	mm->writeValue(p1 + 1, Value::Pointer(p2));

	// alloc unreachable
	mm->writeValue(mm->allocate(4), Value::Number(3));
	mm->writeValue(mm->allocate(4), Value::Number(4));
	mm->writeValue(mm->allocate(4), Value::Number(5));

	log("\nBefore GC:", "");

	mm->dump();

	auto gcStats = mm->collect();

	log("    total:", gcStats->total);
	log("    alive:", gcStats->alive);
	log("reclaimed:", gcStats->reclaimed);				// will include every unreachalbe block including the empty space at then end of heap...even if we didn't allocate it, its not reachable from the root and will be reclainmed.

	log("\nAfter GC:", "");
	mm->dump();
} 