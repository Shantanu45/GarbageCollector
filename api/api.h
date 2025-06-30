
#ifdef GCAPI_EXPORTS
#define VIEW_INTERFACE __declspec(dllexport)
#else
#define VIEW_INTERFACE __declspec(dllimport)
#endif

#include <stdint.h>

#include <vector>
#include <cstring> // for strdup
#include "Stats/HeapStats.h"
#include "Stats/GCStats.h"
#include "Utils/alloc-util.h"

// =============== Bindings ======================

struct GCStats_API {
    uint32_t total;
    uint32_t alive;
    uint32_t reclaimed;
    const char* name;
    const char* allocatorName;
};

struct HeapData_API
{
    uint32_t from;    // key from map (used location start)
    uint32_t size;    // this size include header data size as well
    const char* name;  // pointer to null-terminated C string
    bool deleted;
};

struct HeapStats_API {
    uint32_t totalSize;
    uint32_t freeSpace;

    // Pointer to array of HeapData
    HeapData_API* usedLocations;
    uint32_t usedLocationsCount;
};


HeapStats_API* ConvertToCCompatible_HeapStats(const HeapStats& original);

void FreeHeapStatsC(HeapStats_API& stats);

GCStats_API* CreateGCStats(const GCStats stat);

HeapStats_API* h_state;
GCStats_API* gc_state;

VIEW_INTERFACE std::shared_ptr<MemoryManager> GGetActiveMemoryManager_API();

VIEW_INTERFACE bool GSetActiveMemoryManager_API(std::shared_ptr<MemoryManager> mm);

extern "C"
{
    VIEW_INTERFACE bool RunSample();

    VIEW_INTERFACE bool UpdateStats();

    VIEW_INTERFACE HeapStats_API* GetHeapStats();
    VIEW_INTERFACE bool FreeHeapStats();

    VIEW_INTERFACE GCStats_API* GetGCStats();
    VIEW_INTERFACE bool FreeGCStats();

}