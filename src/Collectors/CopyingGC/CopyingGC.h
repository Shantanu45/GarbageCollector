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

    void forward();

    void fixPointer();

    void swap();

    std::shared_ptr<Heap> ToHeap;
    std::shared_ptr<Heap> FromHeap;

    std::shared_ptr<IAllocator> ToAllocator;
    std::shared_ptr<IAllocator> FromAllocator;

    uint32_t totalAllocSize;
private:
};