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
	auto gc_logger = spdlog::stdout_color_mt("GC_STAT");			// stdout_color_st() for single threaded
	gc_logger->set_pattern("[%H:%M:%S] [%^%n%$] %v");
	auto heap_stat_logger = spdlog::stdout_color_mt("HEAP_STAT");
	heap_stat_logger->set_pattern("[%H:%M:%S] [%^%n%$] %v");
	auto heap_dump_looger = spdlog::stdout_color_mt("HEAP_DUMP");
	heap_dump_looger->set_pattern("[%^%n%$] %v");
}

inline void resetLogger()
{
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
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
	out << "\033[33;1m[";
	std::vector<uint32_t> heapState(heapStats->size, 0);

	bool flag = true;
	for (auto& u : heapStats->usedLocations)
	{
		for (size_t i = u.from; i < u.from+u.size; i++)
		{
			heapState[i] = flag?1:2;
		}
		flag = !flag;
	}
	flag = true;
	for (auto s: heapState)
	{
		if (s == 0)
		{
			out << "\033[0m";
			out << "-";
		}
		else if(s == 1){
			if (flag == false)
			{
				out << "\033[0m";
				out << "\033[32;1m";
				flag = true;
			}
			out << "=";
		}
		else if (s == 2) {
			if (flag == true)
			{
				out << "\033[0m";
				out << "\033[31;1m";
				flag = false;
			}
			out << "=";
		}
	}
	//for (size_t i = 0; i < heapStats->size; i++)
	//{
	//	bool used = false;
	//	for (auto& u : heapStats->usedLocations)
	//	{
	//		if (i >= u.from && i < u.from + u.size)
	//		{
	//			used = true;
	//			break;  // no need to check further
	//		}
	//	}

	//	if (used)
	//		out << "=";
	//	else
	//		out << "-";
	//}
	out << "]\033[0m";
	logger->info("Heap usage: {}", out.str());
}

inline void dumpHeapContents(Word* heapStart, uint32_t wordsCount)
{
	auto logger = spdlog::get("HEAP_DUMP");
	logger->info("\033[33;1m--START --\033[0m");
	//spdlog::set_pattern("%^%v%$");

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

	logger->info("\033[33;1m-- END --\033[0m");

	//logger->info("bin {}", spdlog::to_hex(heapStart, wordsCount * sizeof(Word)));
}