#pragma once

#include <stdint.h>
#include <memory>
#include <unordered_map>
#include <string>

struct HeapStats
{
    struct data
    {
        uint32_t size;
        std::string name;

        bool operator==(const data& other) const {
            return size == other.size && name == other.name;
        }
    };

    uint32_t totalSize;

    // from -> data (used block)
    std::unordered_map<uint32_t, data> usedLocations;

    void MarkUsed(uint32_t from, uint32_t size, const std::string& name = "")
    {
        usedLocations.emplace(from, data{ size, name });
    }

    void MarkUnUsed(uint32_t from)
    {
        usedLocations.erase(from);
    }

    void Reset()
    {
        usedLocations.clear();
    }
};