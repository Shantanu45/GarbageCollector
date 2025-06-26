#pragma once

#include <list>
#include <memory>

#include "../ICollector.h"

#include "../../Value/Value.h"
#include "../../Allocators/IAllocator.h"

#ifdef ENABLED_GC_TEST

#include "gtest/gtest.h"; 
namespace GC_TEST {
    class CopyingGCTest_API_Test;
    class CopyingGCTest_Copy_Test;
    class CopyingGCTest_Collect_Test;
}

#endif


/**
 * Copying garbage collector.
 *
 *   - Copy phase: trace, marks reachable objects as alive
 *   - Forward phase: reclaims memory occupied by unreachable objects
 *   - Fix pointers phase:
 *   - Swap phase:
 *
 * Collects stats during swap.
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

        totalAllocSize = ToHeap->size() + ToHeap->size();
    };

    /**
     * Main collection cycle.
     */
    std::shared_ptr<GCStats> collect();


private:

#ifdef ENABLED_GC_TEST
    FRIEND_TEST(GC_TEST::CopyingGCTest, API);
    FRIEND_TEST(GC_TEST::CopyingGCTest, Copy);
    FRIEND_TEST(GC_TEST::CopyingGCTest, Collect);
#endif

    void copy();

    Word copyBlock(Word src);

    void forward(Word src, Word dst);

    void fixPointer(Value* ptr, Word src);

    void swap();

    bool isVirtualAddressInFromHeap(Word src);
    bool isVirtualAddressInToHeap(Word src);

    uint16_t virtualAddressRelativeToToHeap(Word src);
    uint16_t virtualAddressRelativeToFromHeap(Word src);

    bool isForwardPointingToSwapHeap(Word src);

    std::shared_ptr<Heap> ToHeap;
    std::shared_ptr<Heap> FromHeap;

    std::shared_ptr<IAllocator> ToAllocator;
    std::shared_ptr<IAllocator> FromAllocator;

    uint32_t totalAllocSize;
    Word dstAlloc = 0;


};