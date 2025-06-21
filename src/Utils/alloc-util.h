#pragma once

#include "../MemoryManager/MemoryManager.h"
#include "../Allocators/FreeList/FreeList.h"
#include "../Collectors/MarkSweepGC/MarkSweepGC.h"
#include "../Collectors/MarkCompactGC/MarkCompactGC.h"

#define log(title, value) std::cout << title << " " << value << std::endl

template <typename T, typename... Args>
inline T* gc_new(std::shared_ptr<MemoryManager> mm, Args&&... args) {
	static_assert(std::is_trivially_destructible<T>::value, "Only trivially destructible types are allowed for now.");

	// Allocate memory for T (in words)
	size_t sizeInBytes = sizeof(T);
	auto addr = mm->allocate(sizeInBytes);

	void* raw = (void*)mm->heap->asWordPointer(addr);
	// Construct object in place using placement new
	//void* raw = reinterpret_cast<void*>(&mm->heap[addr]);
	T* obj = new (raw) T(std::forward<Args>(args)...);

	return obj;
}

template <typename T>
inline void gc_delete(T* ptr) {
	// No-op in GC systems unless you want to trigger destruction manually
	// Or you can use this to support finalization
	// ptr->~T(); // optional
}