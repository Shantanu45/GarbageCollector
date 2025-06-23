#include <stdint.h>
#include <memory>
#include <set>

/**
 * Stats for the collection cycle.
 */
struct GCStats {
	/**
	 * The snapshot of the total objects on the heap before collection.
	 */
	uint32_t total;

	/**
	 * Number of alive (reachable) objects.
	 */
	uint32_t alive;

	/**
	 * Number of reclaimed objects.
	 */
	uint32_t reclaimed;
};