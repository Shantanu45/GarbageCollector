#pragma once

#include <list>
#include <memory>

#include "../ICollector.h"

#include "../../Value/Value.h"
#include "../../Allocators/IAllocator.h"


/**
 * Mark-Sweep garbage collector.
 *
 *   - Mark phase: trace, marks reachable objects as alive
 *   - Sweep phase: reclaims memory occupied by unreachable objects
 *
 * Collects stats during collection.
 *
 */
class CopyingGC : public ICollector {
public:

    CopyingGC(const std::shared_ptr<IAllocator>& allocatorOne, const std::shared_ptr<IAllocator>& allocatorTwo)
        : ICollector(allocatorOne) {
        FromAllocator = allocatorOne;
        FromHeap = allocatorOne->heap;
        ToHeap = allocatorTwo->heap;
        ToAllocator = allocatorTwo;

        totalAllocSize = ToHeap->h_size + ToHeap->h_size;
    };

    /**
     * Main collection cycle.
     */
    std::shared_ptr<GCStats> collect();

    void copy();

    Word copyBlock(Word src);

    void forward(Word src, Word dst);

    void fixPointer(Value* ptr, Word src);

    void swap();

    std::shared_ptr<Heap> ToHeap;
    std::shared_ptr<Heap> FromHeap;

    std::shared_ptr<IAllocator> ToAllocator;
    std::shared_ptr<IAllocator> FromAllocator;

    uint32_t totalAllocSize;
    Word dstAlloc = 0;

    bool isVirtualAddressInFromHeap(Word src);
    bool isVirtualAddressInToHeap(Word src);

    uint16_t virtualAddressRelativeToToHeap(Word src);
    uint16_t virtualAddressRelativeToFromHeap(Word src);

    bool isForwardPointingToSwapHeap(Word src);
private:

};