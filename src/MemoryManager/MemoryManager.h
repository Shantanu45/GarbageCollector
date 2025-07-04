#pragma once

#include <functional>
#include <stdint.h>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "../Value/Value.h"
#include "../Utils/number-util.h"

#include "Heap.h"
#include "ObjectHeader.h"

#include "../Allocators/IAllocator.h"
#include "../Collectors/ICollector.h"

#include <type_traits> // for std::is_same

class CopyingGC;

class MemoryManager {
public:
	std::shared_ptr<Heap> heap;

	std::shared_ptr<IAllocator> allocator;

	std::shared_ptr<ICollector> collector;

	MemoryManager(
		const std::shared_ptr<Heap> heap,
		const std::shared_ptr<IAllocator> allocator,
		const std::shared_ptr<ICollector> collector = nullptr)
		: heap(heap),
		allocator(allocator),
		collector(collector)
	{
		reset();
	}

	template <class Allocator, class Collector, uint32_t heapSize>
	static std::shared_ptr<MemoryManager> create() 
	{
		auto heap = std::make_shared<Heap>(heapSize);
		auto allocator = std::make_shared<Allocator>(heap);
		std::shared_ptr<Collector> collector;

		if constexpr (std::is_same<Collector, CopyingGC>::value) {
			auto heapTwo = std::make_shared<Heap>(heapSize);
			auto allocatorTwo = std::make_shared<Allocator>(heapTwo);
			collector = std::make_shared<Collector>(allocator, allocatorTwo);

		}
		else
		{
			collector = std::make_shared<Collector>(allocator);
		}

		return std::make_shared<MemoryManager>(heap, allocator, collector);
	}

	template <class Allocator, uint32_t heapSize>
	static std::shared_ptr<MemoryManager> create() 
	{
		auto heap = std::make_shared<Heap>(heapSize);
		auto allocator = std::make_shared<Allocator>(heap);

		return std::make_shared<MemoryManager>(heap, allocator, nullptr);
	}

	void reset();

	uint32_t getHeapSize();

	uint32_t getWordsCount();

	uint32_t getWordSize();

	uint8_t* asBytePointer(Word address);

	Word* asWordPointer(Word assress);

	Word toVirtualAddress(void* assress);

	void writeWord(uint32_t address, uint32_t value);

	Word readWord(Word address);

	void writeByte(Word address, uint8_t value);

	uint8_t readByte(Word address);

	void writeValue(Word address, uint32_t value, Type valueType);

	void writeValue(Word address, Value& value);

	void writeValue(Word address, Value&& value);

	Value* readValue(Word address);

	Value allocate(uint32_t n, std::string name = "");

	void free(Word address);

	std::shared_ptr<GCStats> collect();

	ObjectHeader* getHeader(Word address);

	uint16_t sizeOf(Word address);

	void dump();

	std::vector<Value*> getPointers(Word address);

	uint32_t getObjectCount();

	std::shared_ptr<GCStats> getGCStats();

	std::shared_ptr<HeapStats> getHeapStats();

	void makeRoot(Word w) {
		collector->makeRoot(w);
	}
private:

	// usefull when copy collector is running.
	void RefreshAllocatorReference()
	{
		allocator = collector->allocator;
	}

	// usefull when copy collector is running.
	void RefreshHeapReference()
	{
		heap = allocator->heap;
	}

};