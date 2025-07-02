#pragma once

#include <stdint.h>
#include <memory>
#include <vector>

#include "../Stats/GCStats.h"
#include "../MemoryManager/Heap.h"
#include "GCHelpers.h"
#include "../Utils/timer-util.h"


/**
 * Abstract collector class.
 *
 * Used as a base class for all Garbage collection classes.
 *
 * Works in pair with the associated allocator, and shares the heap
 * with this allocator.
 */
class ICollector {
 public:
  /**
   * Associated allocator.
   */
  std::shared_ptr<IAllocator> allocator;

  /**
   * Stats for the collection cycle.
   */
  std::shared_ptr<GCStats> stats;

  ICollector(std::shared_ptr<IAllocator> allocator)
      : allocator(allocator), stats(std::make_shared<GCStats>()) 
  {
      // TODO: impelement actual roots, use first block for now.
      stats->allocatorName = allocator->GetAllocatorName();

  }

  virtual ~ICollector() {}

  /**
   * Executes a collection cycle.
   */
  virtual std::shared_ptr<GCStats> collect() = 0;

  /**
   * Returns GC roots.
   */
  std::vector<Word>& getRoots() {  
    return _roots;
  }

  void setRoots(std::vector<Word>& newRoots)
  {
      _roots = newRoots;
  }

  /**
   * Initializes the collector for the cycle.
   */
  void init() {
    _resetStats();
  }

  virtual std::string getCollectorName() = 0;

  void makeRoot(Word r) {
      _roots.push_back(r);
  }

  GCTimerManager profiler;
 protected:
  /**
   * Resets the GC stats.
   */
  void _resetStats() {
    stats->total = allocator->getObjectCount();
    stats->alive = 0;
    stats->reclaimed = 0;
  }

  std::vector<Word> _roots;

};
