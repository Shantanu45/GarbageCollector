
#include <iostream>
#include <memory>
#include "Utils/alloc-util.h"
#include "Value/Value.h"

void printGCStats(std::shared_ptr<MemoryManager> mm)
{
	std::shared_ptr<GCStats> gcStats = mm->getGCStats();
	log("    total:", gcStats->total);
	log("    alive:", gcStats->alive);
	log("reclaimed:", gcStats->reclaimed);				// will include every unreachalbe block including the empty space at then end of heap...even if we didn't allocate it, its not reachable from the root and will be reclainmed.
}

void sample_1()
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

	printGCStats(mm);

	log("\nAfter GC:", "");
	mm->dump();
}

struct MyObject
{
	Value val;
	Value ptr;
};

void sample_2()
{
	std::shared_ptr<MemoryManager> mm = MemoryManager::create<FreeListAllocator, MarkSweepGC, 64>();

	GSetActiveMemoryManager(mm);

	MyObject* obj = gc_new <MyObject>();

	obj->val = Value::Number(42);

	MyObject* obj2 = gc_new<MyObject>();

	obj2->val = Value::Number(24);
	obj2->ptr = Value::Pointer(mm->allocate(4));

	obj->ptr = Value::Pointer(mm->toVirtualAddress(obj2));

	mm->writeValue(obj->ptr, Value::Number(45));

	//gc_delete(obj->ptr);

	log("MyObj Value", obj->val.decode());

	log("\nBefore GC:", "");

	mm->dump();

	printGCStats(mm);

	auto gcStats = mm->collect();

	printGCStats(mm);

	log("\nAfter GC:", "");
	mm->dump();
}

int main()
{
	sample_2();
} 