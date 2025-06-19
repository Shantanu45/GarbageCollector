#include <array>
#include <iostream>
#include <memory>

#include "MemoryManager/MemoryManager.h"
#include "MemoryManager/ObjectHeader.h"
#include "Allocators/FreeList/FreeList.h"
#include "Value/Value.h"
#include "Utils/number-util.h"

#define log(title, value) std::cout << title << " " << value << std::endl
#define log(text) std::cout << text << std::endl

int main()
{
	std::shared_ptr<MemoryManager> mm = MemoryManager::create<SingleFreeListAllocator, 64>();

	Value p1 = mm->allocate(12);

	mm->writeWord(p1, Value::Number(1));
	mm->writeValue(p1 + 2, Value::Pointer(p1));

	auto p2 = mm->allocate(4);

	mm->writeValue(p2, Value::Number(2));
	mm->writeValue(p1 + 1, Value::Pointer(p2));

	log("p1 Number", mm->readValue(p1)->decode());

	mm->free(p2);
	mm->free(p1);


	// alloc unreachable
	//mm->writeValue(mm->allocate(4), Value::Number(3));
	//mm->writeValue(mm->allocate(4), Value::Number(4));
	//mm->writeValue(mm->allocate(4), Value::Number(5));
} 