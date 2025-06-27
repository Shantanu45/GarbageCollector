
#ifdef GCAPI_EXPORTS
#define VIEW_INTERFACE __declspec(dllexport)
#else
#define VIEW_INTERFACE __declspec(dllimport)
#endif

#include <stdint.h>
// =============== Bindings ======================

struct GCStats_API {
    uint32_t total;
    uint32_t alive;
    uint32_t reclaimed;
};

/*

[StructLayout(LayoutKind.Sequential)]
public struct GCStats
{
    public uint total;
    public uint alive;
    public uint reclaimed;
}

*/

struct HeapData_API
{
    uint32_t from;    // key from map (used location start)
    uint32_t size;    // this size include header data size as well
    const char* name;  // pointer to null-terminated C string
    bool deleted;
};

struct HeapStats_API {
    uint32_t totalSize;

    // Pointer to array of HeapData
    HeapData_API* usedLocations;
    uint32_t usedLocationsCount;
};

/*

[StructLayout(LayoutKind.Sequential)]
public struct HeapData
{
    public uint size;

    // Pointer to null-terminated ANSI string
    public IntPtr name;

    [MarshalAs(UnmanagedType.I1)] // bool is 1 byte in C++
    public bool deleted;
}

[StructLayout(LayoutKind.Sequential)]
public struct HeapStats
{
    public uint totalSize;

    public IntPtr usedLocations; // pointer to HeapData array
    public uint usedLocationsCount;
}

*/

#include <vector>
#include <cstring> // for strdup
#include "Stats/HeapStats.h"
#include "Stats/GCStats.h"
#include "Utils/alloc-util.h"

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