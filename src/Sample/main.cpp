
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

	MyObject2* obj = gc_new <MyObject2>("Obj1");

	obj->val = Value::Number(42);

	MyObject* obj2 = gc_new<MyObject>("Obj2");

	auto ptr = mm->allocate(4, "Obj3");

	obj2->val = Value::Number(24);
	obj2->ptr = Value::Pointer(ptr);

	obj->ptr = Value::Pointer(mm->toVirtualAddress(obj2));
	obj->ptr2 = Value::Pointer(nullptr);

	mm->writeValue(obj->ptr, Value::Number(45));

	//obj->obj2->ptr
	spdlog::info("Initial Heap state...");
	printHeapStats(mm->allocator->heapStats);

	spdlog::info("Deleting obj2...");
	gc_delete(mm->toVirtualAddress(obj2));

	//log("MyObj Value", obj->val.decode());

	spdlog::info("Before GC:");

	mm->dump();

	printHeapStats(mm->allocator->heapStats);

	auto gcStats = mm->collect();
	spdlog::info("\After GC:");

	printGCStats(gcStats);
	printHeapStats(mm->allocator->heapStats);

	mm->dump();
}

void sample_3() {
	static auto heap = std::make_shared<Heap>(32);
	static auto allocator = std::make_shared<FreeListAllocator>(heap);
	MarkCompactGC cgc(allocator);
}

int main()
{
	setupLogger();
	sample_2();
} 