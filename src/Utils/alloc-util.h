#pragma once

#include "../MemoryManager/MemoryManager.h"
#include "../Allocators/FreeList/FreeList.h"
#include "../Collectors/MarkSweepGC/MarkSweepGC.h"
#include "../Collectors/MarkCompactGC/MarkCompactGC.h"

//#define log(title, value) std::cout << title << " " << value << std::endl

inline std::shared_ptr<MemoryManager> GActiveMemoryManager = nullptr;

inline bool GSetActiveMemoryManager(std::shared_ptr<MemoryManager> mm)
{
	GActiveMemoryManager = mm;
	return true;
}


template <typename T, typename... Args>
inline T* gc_new(std::string name = "", Args&&... args) {
	if (!GActiveMemoryManager)
	{
		static_assert("No registered Memory Manager found. Please set GActiveMemoryManager via GSetActiveMemoryManager()");
		return nullptr;
	}
	static_assert(std::is_trivially_destructible<T>::value, "Only trivially destructible types are allowed for now.");

	// Allocate memory for T (in words)
	size_t sizeInBytes = sizeof(T);
	auto addr = GActiveMemoryManager->allocate(sizeInBytes);

	void* raw = (void*)GActiveMemoryManager->heap->asWordPointer(addr);
	// Construct object in place using placement new
	//void* raw = reinterpret_cast<void*>(&GActiveMemoryManager->heap[addr]);
	T* obj = new (raw) T(std::forward<Args>(args)...);

	return obj;
}


template <typename T>
inline T* gc_new(std::string name = "") {
	if (!GActiveMemoryManager)
	{
		static_assert("No registered Memory Manager found. Please set GActiveMemoryManager via GSetActiveMemoryManager()");
		return nullptr;
	}
	static_assert(std::is_trivially_destructible<T>::value, "Only trivially destructible types are allowed for now.");

	// Allocate memory for T (in words)
	uint32_t sizeInBytes = sizeof(T);
	auto addr = GActiveMemoryManager->allocate(sizeInBytes, name);

	void* raw = (void*)GActiveMemoryManager->heap->asWordPointer(addr);
	// Construct object in place using placement new

	T* obj = new (raw) T();

	return obj;
}


template <typename T>
inline void gc_delete(T* ptr) {
	if (!GActiveMemoryManager)
	{
		static_assert("No registered Memory Manager found. Please set GActiveMemoryManager via GSetActiveMemoryManager()");
		return;
	}
	auto v_ptr = GActiveMemoryManager->heap->asVirtualAddress(ptr);
	GActiveMemoryManager->free(v_ptr);
	// No-op in GC systems unless you want to trigger destruction manually
	// Or you can use this to support finalization
	// ptr->~T(); // optional
}

inline void gc_delete(Value ptr) {
	if (!GActiveMemoryManager)
	{
		static_assert("No registered Memory Manager found. Please set GActiveMemoryManager via GSetActiveMemoryManager()");
		return;
	}
	GActiveMemoryManager->free(ptr.decode());
		// No-op in GC systems unless you want to trigger destruction manually
		// Or you can use this to support finalization
		// ptr->~T(); // optional
}