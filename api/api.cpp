
#include "api.h"
#include "../src/Examples.h"
#include "../src/Collectors/CopyingGC/CopyingGC.h"

HeapStats_API* ConvertToCCompatible_HeapStats(const HeapStats& original)
{
    HeapStats_API* result = new HeapStats_API;
    result->totalSize = original.totalSize;

    // Allocate vector of HeapData to hold all usedLocations entries
    std::vector<HeapData_API> dataVector;
    dataVector.reserve(original.usedLocations.size());

    for (const auto& kv : original.usedLocations)
    {
        HeapData_API hd;
        hd.from = kv.first;
        hd.size = kv.second.size;
        hd.deleted = kv.second.deleted;

        // strdup allocates new null-terminated C string copy of std::string
        hd.name = _strdup(kv.second.name.c_str());

        dataVector.push_back(hd);
    }

    // Allocate array on heap, caller must free
    HeapData_API* array = new HeapData_API[dataVector.size()];
    std::memcpy(array, dataVector.data(), sizeof(HeapData_API) * dataVector.size());

    result->usedLocations = array;
    result->usedLocationsCount = static_cast<uint32_t>(dataVector.size());

    return result;
}

void FreeHeapStatsC(HeapStats_API& stats)
{
    for (uint32_t i = 0; i < stats.usedLocationsCount; ++i)
    {
        free((void*)stats.usedLocations[i].name); // free strdup'ed strings
    }
    delete[] stats.usedLocations;
    stats.usedLocations = nullptr;
    stats.usedLocationsCount = 0;
}

GCStats_API* CreateGCStats(const GCStats stat)
{
    GCStats_API* stats = new GCStats_API();
    stats->total = stat.total;
    stats->alive = stat.alive;
    stats->reclaimed = stat.reclaimed;
    stats->name = _strdup(stat.name.c_str());
    stats->allocatorName = _strdup(stat.allocatorName.c_str());
    return stats;
}

std::shared_ptr<MemoryManager> GGetActiveMemoryManager_API()
{
    return GGetActiveMemoryManager();
}

bool GSetActiveMemoryManager_API(std::shared_ptr<MemoryManager> mm)
{
    return GSetActiveMemoryManager(mm);
}

extern "C" {
    bool RunSample()
    {
        sample_2();
        return true;
    }

    bool UpdateStats()
    {
        std::shared_ptr<const HeapStats> tempHeapStat;
        std::shared_ptr<const GCStats> tempGCStat;
        auto mm = GGetActiveMemoryManager_API();
        if (mm == nullptr)
        {
            return false;
        }

        tempHeapStat = mm->getHeapStats();
        tempGCStat = mm->getGCStats();

        if (tempHeapStat == nullptr || tempGCStat == nullptr)
        {
            return false;
        }
        h_state = ConvertToCCompatible_HeapStats(*tempHeapStat);
        gc_state = CreateGCStats(*tempGCStat);
        return true;
    }

    HeapStats_API* GetHeapStats()
    {
        return h_state;
    }
    bool FreeHeapStats()
    {
        FreeHeapStatsC(*h_state);
        delete h_state;
        h_state = nullptr;
        return true;
    }
    GCStats_API* GetGCStats()
    {
        return gc_state;
    }
    bool FreeGCStats()
    {
        delete gc_state;
        gc_state = nullptr;
        return true;
    }
}
