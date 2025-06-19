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
class MarkSweepGC : public ICollector {
public:
    MarkSweepGC(const std::shared_ptr<IAllocator>& allocator)
        : ICollector(allocator) {
    };

    /**
     * Main collection cycle.
     */
    std::shared_ptr<GCStats> collect();

    /**
     * Mark phase. Returns number of live objects.
     */
    void mark();

    /**
     * Sweep phase. Resets the mark bit, reclaims the objects by
     * adding back to the free list.
     */
    void sweep();
};