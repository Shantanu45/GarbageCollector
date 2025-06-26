#include "MemoryManager.h"

void MemoryManager::reset() {
	heap->reset();
	allocator->reset();
}

uint32_t MemoryManager::getHeapSize() { return heap->size(); }

uint32_t MemoryManager::getWordsCount() { return heap->size() / getWordSize(); }

uint32_t MemoryManager::getWordSize() { return sizeof(Word); }

uint8_t* MemoryManager::asBytePointer(Word address) {
	return heap->asBytePointer(address);
}

Word* MemoryManager::asWordPointer(Word address) {
	return heap->asWordPointer(address);
}

Word MemoryManager::toVirtualAddress(void* ptr)
{
	return heap->asVirtualAddress((Word*)ptr);
}

void MemoryManager::writeWord(Word address, Word value) {
	*asWordPointer(address) = value;
}

uint32_t MemoryManager::readWord(Word address) {
	return *asWordPointer(address);
}

void MemoryManager::writeByte(Word address, uint8_t value) {
	(*heap)[address] = value;
}

uint8_t MemoryManager::readByte(Word address) { return (*heap)[address]; }

void MemoryManager::writeValue(Word address, Word value, Type valueType) {
	writeWord(address, Value::encode(value, valueType));
}

void MemoryManager::writeValue(uint32_t address, Value& value) {
	writeWord(address, value);
}

void MemoryManager::writeValue(uint32_t address, Value&& value) {
	writeWord(address, value);
}

Value* MemoryManager::readValue(uint32_t address) {
	return (Value*)asWordPointer(address);
}

Value MemoryManager::allocate(uint32_t n, std::string name) { return allocator->allocate(n, name); }

void MemoryManager::free(Word address) { 
	//auto header = getHeader(address);
	//header->mark = 2;		// 2 means freed explicitly by user.
	allocator->free(address); 
}

std::shared_ptr<GCStats> MemoryManager::collect() {
	if (!collector) {
		throw std::runtime_error("Collector is not specified.");
	}

	return collector->collect();
}

ObjectHeader* MemoryManager::getHeader(Word address) {
	return allocator->getHeader(address);
}

uint16_t MemoryManager::sizeOf(Word address) {
	return getHeader(address)->size;
}

std::vector<Value*> MemoryManager::getPointers(Word address) {
	return allocator->getPointers(address);
}

uint32_t MemoryManager::getObjectCount() { return allocator->getObjectCount(); }

void MemoryManager::dump() { heap->dump(); }

std::shared_ptr<GCStats> MemoryManager::getGCStats()
{
	return collector->stats;
}

std::shared_ptr<HeapStats> MemoryManager::getHeapStats()
{
	return allocator->heapStats;
}