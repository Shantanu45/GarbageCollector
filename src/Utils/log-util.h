#pragma once

#ifdef CPP_CONSOLE_CLIENT
#define ALLOW_SPDLOG
#endif

#include <iterator>
#include "../Utils/number-util.h"
#include "spdlog/spdlog.h"
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/fmt/bin_to_hex.h"
#include "../Stats/GCStats.h"
#include "../Stats/HeapStats.h"
#include "common.h"


inline void setupLogger()
{
#ifdef ALLOW_SPDLOG
	auto gc_logger = spdlog::stdout_color_mt("GC_STAT");			// stdout_color_st() for single threaded
	gc_logger->set_pattern("[%H:%M:%S] [%^%n%$] %v");
	auto heap_stat_logger = spdlog::stdout_color_mt("HEAP_STAT");
	heap_stat_logger->set_pattern("[%H:%M:%S] [%^%n%$] %v");
	auto heap_dump_looger = spdlog::stdout_color_mt("HEAP_DUMP");
	heap_dump_looger->set_pattern("[%^%n%$] %v");
#endif
}

inline void resetLogger()
{
#ifdef ALLOW_SPDLOG
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
#endif
}

inline void printGCStats(std::shared_ptr<GCStats> gcStats)
{
#ifdef ALLOW_SPDLOG
	auto logger = spdlog::get("GC_STAT");

	logger->info("    total:{}", gcStats->total);
	logger->info("    alive:{}", gcStats->alive);
	logger->info("reclaimed:{}", gcStats->reclaimed);				// will include every unreachalbe block including the empty space at then end of heap...even if we didn't allocate it, its not reachable from the root and will be reclainmed.
	//spdlog::drop("GC");
	//spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
	//spdlog::set_level(spdlog::level::info);   // Set new global log level
#endif
}

inline void printHeapStats(std::shared_ptr<HeapStats> heapStats)
{
#ifdef ALLOW_SPDLOG
    auto logger = spdlog::get("HEAP_STAT");
    std::ostringstream out;
    out << "\033[0m[";  // Reset color, start of heap line

    // Initialize heap state with placeholder characters
    std::vector<std::string> heapState(heapStats->totalSize, "-");

	for (auto& u : heapStats->usedLocations)
	{
		for (size_t i = u.first; i < u.first + u.second.size; i++)
		{
			if (u.second.name != "")
			{
				heapState[u.first] = u.second.name + "(" + std::to_string(u.second.size) + ")";
				for (size_t i = u.first + 1; i < u.first + u.second.size; i++)
				{
					heapState[i] = "";
				}
			}
			else
			{
				for (size_t i = u.first; i < u.first + u.second.size; i++)
				{
					heapState[i] = "=";
				}
			}
		}
	}

    // Output with colors
    for (const auto& s : heapState)
    {
        if (s == "-")
        {
            out << "\033[0m" << "-";
        }
        else if (s == "=")
        {
            out << "\033[32;1m" << "=";  // Green for unnamed used block
        }
        else
        {
            out << "\033[33;1m" << s;    // Yellow for labeled block
        }
    }

    out << "]\033[0m";  // Reset at the end
    logger->info("Heap usage: {}", out.str());

	logger->info("TotalSize:{} FreeSpace:{}", heapStats->totalSize, heapStats->freeSpace);
#endif
}

inline void dumpHeapContents(Word* heapStart, uint32_t wordsCount)
{

	auto logger = spdlog::get("HEAP_DUMP");
	logger->info("\033[33;1m--START --\033[0m");
	//spdlog::set_pattern("%^%v%$");

	int address = 0;

	auto words = heapStart;

	for (uint32_t i = 0; i < wordsCount; i++) {
		std::string row = "";

		row += int_to_hex(address) + " : ";

		auto value = int_to_hex(words[i], /*usePrefix*/ false);
		insert_delimeter(value, 2, " ");

		row += value;

		logger->info(row);

		address += sizeof(Word);
	}

	logger->info("\033[33;1m-- END --\033[0m");


	//logger->info("bin {}", spdlog::to_hex(heapStart, wordsCount * sizeof(Word)));
}