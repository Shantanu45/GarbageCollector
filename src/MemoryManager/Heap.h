#pragma once

#include "../Utils/log-util.h"
#include "../Utils/common.h"

/**
 * Virtual heap storage with convenient methods of converting
 * between physical, and virtual pointers.
 */
struct Heap {
	uint8_t* storage;
	uint32_t h_size;

	Heap(uint32_t size) : h_size(size) {
		storage = reinterpret_cast<uint8_t*>(malloc(size));			// remeber to dealloc after done.
		memset(storage, 0x0, h_size);
	}

	uint8_t& operator[](int offset) { return storage[offset]; }

	/**
	 * Returns the size of the heap.
	 */
	uint32_t size() { return h_size; }

	/**
	 * Returns an actual Word pointer for the virtual pointer address.
	 */
	Word* asWordPointer(Word address) { 
		return (Word*)&storage[address]; 
	}

	/**
	 * Returns an actual byte pointer for the virtual pointer address.
	 */
	uint8_t* asBytePointer(Word address) { 
		return &storage[address]; 
	}

	/**
	 * Converts an actual Word pointer to the virtual address.
	 */
	Word asVirtualAddress(Word* address) {
		return (Word)((uint8_t*)address - asBytePointer(0));
	}

	/**
	 * Resets this heap.
	 */
	void reset() { memset(&storage[0], 0, h_size); }

	/**
	 * Dumps the heap memory.
	 */
	void dump() {
		dumpHeapContents(asWordPointer(0), size() / sizeof(Word));
	}

	~Heap() {
		delete storage;
	}
	
private:

};