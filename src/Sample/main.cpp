
#include "Utils/alloc-util.h"
#include "Utils/log-util.h"

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

	//log("\nBefore GC:", "");

	mm->dump();

	auto gcStats = mm->collect();
	printGCStats(gcStats);

	//log("\nAfter GC:", "");
	mm->dump();
}

struct MyObject
{
	Value val;
	Value ptr;
};

struct MyObject2
{
	Value val;
	Value ptr;
	Value ptr2;
};

void sample_2()
{
	std::shared_ptr<MemoryManager> mm = MemoryManager::create<FreeListAllocator, MarkCompactGC, 64>();

	GSetActiveMemoryManager(mm);

	MyObject2* obj = gc_new <MyObject2>("Obj");

	obj->val = Value::Number(42);

	MyObject* obj2 = gc_new<MyObject>("Obj2");

	auto ptr = mm->allocate(4, "FOUR");

	obj2->val = Value::Number(24);
	obj2->ptr = Value::Pointer(ptr);

	obj->ptr = Value::Pointer(mm->toVirtualAddress(obj2));
	obj->ptr2 = Value::Pointer(ptr);

	mm->writeValue(obj->ptr, Value::Number(45));

	//obj->obj2->ptr
	printHeapStats(mm->allocator->heapStats);

	//gc_delete(mm->toVirtualAddress(obj2));

	//log("MyObj Value", obj->val.decode());

	spdlog::info("\nBefore GC:");

	mm->dump();

	printHeapStats(mm->allocator->heapStats);
	auto gcStats = mm->collect();

	printGCStats(gcStats);
	printHeapStats(mm->allocator->heapStats);


	spdlog::info("\After GC:");

	mm->dump();
}

int main()
{
	setupLogger();
	sample_2();
} 