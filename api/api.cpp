
#include "api.h"



HeapStats_API ConvertToCCompatible_HeapStats(const HeapStats& original)
{
    HeapStats_API result;
    result.totalSize = original.totalSize;

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

    result.usedLocations = array;
    result.usedLocationsCount = static_cast<uint32_t>(dataVector.size());

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

GCStats_API CreateGCStats(const GCStats stat)
{
    GCStats_API stats;
    stats.total = stat.total;
    stats.alive = stat.alive;
    stats.reclaimed = stat.reclaimed;
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

    HeapStats_API GetHeapStats()
    {
        return h_state;
    }
    bool FreeHeapStats()
    {
        FreeHeapStatsC(h_state);
        return true;
    }
    GCStats_API GetGCStats()
    {
        return gc_state;
    }
    bool FreeGCStats()
    {
        gc_state = {};
        return true;
    }
}
