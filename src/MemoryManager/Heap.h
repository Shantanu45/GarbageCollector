#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>

#include "../Utils/number-util.h"
#include "spdlog/spdlog.h"

 /**
  * 32 bit machine word.
  */
using Word = uint32_t;

/**
 * Virtual heap storage with convenient methods of converting
 * between physical, and virtual pointers.
 */
struct Heap {
	uint8_t* storage;
	size_t h_size;

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
		return (uint8_t*)address - asBytePointer(0);
	}

	/**
	 * Resets this heap.
	 */
	void reset() { memset(&storage[0], 0, h_size); }

	/**
	 * Dumps the heap memory.
	 */
	void dump() {

		spdlog::info("\nMemory dump:------------------------\n");

		int address = 0;

		auto words = asWordPointer(0);
		auto wordsCount = size() / sizeof(Word);

		for (auto i = 0; i < wordsCount; i++) {
			std::string row = "";

			row += int_to_hex(address) + " : ";

			auto value = int_to_hex(words[i], /*usePrefix*/ false);
			insert_delimeter(value, 2, " ");

			row += value;

			spdlog::info(row);

			address += sizeof(Word);
		}

		spdlog::info("");  // blank line
	}

	~Heap() {
		delete storage;
	}
};