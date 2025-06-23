#pragma once

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
	spdlog::stdout_color_mt("GC_STAT");			// stdout_color_st() for single threaded
	spdlog::stdout_color_mt("HEAP_STAT");
	spdlog::stdout_color_mt("HEAP_DUMP");
	spdlog::set_pattern("[%H:%M:%S] [%^%n%$] %v");
}

inline void resetLogger()
{
	spdlog::set_pattern("[%H:%M:%S] [%^%n%$] %v");
	//spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
}

inline void printGCStats(std::shared_ptr<GCStats> gcStats)
{
	auto logger = spdlog::get("GC_STAT");

	logger->info("    total:{}", gcStats->total);
	logger->info("    alive:{}", gcStats->alive);
	logger->info("reclaimed:{}", gcStats->reclaimed);				// will include every unreachalbe block including the empty space at then end of heap...even if we didn't allocate it, its not reachable from the root and will be reclainmed.
	//spdlog::drop("GC");
	//spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
	//spdlog::set_level(spdlog::level::info);   // Set new global log level
}

inline void printHeapStats(std::shared_ptr<HeapStats> heapStats)
{
	auto logger = spdlog::get("HEAP_STAT");
	std::ostringstream out;
	out << "[";
	for (size_t i = 0; i < heapStats->size; i++)
	{
		if (heapStats->usedLocations.find(i) != heapStats->usedLocations.end())
		{
			out << "=";
		}
		else
		{
			out << "-";
		}
	}
	out << "]";
	logger->info("Heap usage: {}", out.str());
}

inline void dumpHeapContents(Word* heapStart, uint32_t wordsCount)
{
	auto logger = spdlog::get("HEAP_DUMP");
	logger->info("--START --");
	spdlog::set_pattern("%v");

	int address = 0;

	auto words = heapStart;

	for (auto i = 0; i < wordsCount; i++) {
		std::string row = "";

		row += int_to_hex(address) + " : ";

		auto value = int_to_hex(words[i], /*usePrefix*/ false);
		insert_delimeter(value, 2, " ");

		row += value;

		logger->info(row);

		address += sizeof(Word);
	}

	resetLogger();
	logger->info("-- END --");

	//logger->info("bin {}", spdlog::to_hex(heapStart, wordsCount * sizeof(Word)));
}