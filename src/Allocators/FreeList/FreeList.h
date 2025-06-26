#pragma once

#include <stdint.h>
#include <list>
#include <vector>

#include "../../Value/Value.h"
#include "../IAllocator.h"

 /**
  *
  * SFL (Single Free List) Allocator.
  *
  * Maintains a linked list of available blocks.
  *
  * On allocation returns a pointer, set to the next byte after the
  * object header. Maintains the Free list abstraction for allocation.
  *
  *  +----+-------+------++---------+-------+
  *  | GC | Used? | Size || Payload | Align |
  *  +----+-------+------++---------+-------+
  *  ^                    ^
  *  ------ Header ------ User pointer
  */
class FreeListAllocator : public IAllocator {
    /**
     * Total object count on the heap.
     */
    uint32_t _objectCount;

    /**
     * Free list: linked list of all free memory chunks.
     */
    std::list<uint32_t> freeList;

public:

    FreeListAllocator(std::shared_ptr<Heap> heap)
        : IAllocator(heap), freeList() {
        reset();
    }

    ~FreeListAllocator() {}

    /**
     * Allocates a memory chunk with an object header.
     * The payload pointer is set to the first byte (after the header).
     *
     * This returns a "virtual pointer" (Value::Pointer), the byte address in
     * the virtual heap. To convert it to C++ pointer, use `asWordPointer(p)` or
     * `asBytePointer(p)`.
     *
     * Value::Pointer(nullptr) payload signals OOM.
     */
    Value allocate(uint32_t n, std::string name = "") override;


    Value allocateWithData(uint32_t n, Word* data, std::string name = "");

    /**
     * Returns the block to the allocator.
     */
    void free(Word address) override;

    /**
      * Move parts of heap to different address
    */
    void relocate(Word To, Word from, size_t size) override;

    /**
     * Resets the allocator.
     */
    void reset() override;

    /**
     * Returns the reference to the object header.
     */
    ObjectHeader* getHeader(Word address);

    /**
     * Returns total amount of objects on the heap.
     */
    uint32_t getObjectCount();

    /**
     * Returns child pointers of this object.
     */
    std::vector<Value*> getPointers(Word address);

    void setFreeTailRegion(Word startVirtualAddress) {
        _setFreeRegion(startVirtualAddress, heap->size());
    }

private:
    void _resetFreeList();
    void _resetFirstBlock();
    void _setFreeRegion(Word from, Word to);

    // keeping track of space that was never allocated. 
    Word _unTouchedSpace = 0;
};
