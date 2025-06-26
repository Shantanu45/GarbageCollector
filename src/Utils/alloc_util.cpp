#include "alloc-util.h"

std::shared_ptr<MemoryManager> GActiveMemoryManager = nullptr;

bool GSetActiveMemoryManager(std::shared_ptr<MemoryManager> mm)
{
    GActiveMemoryManager = mm;
    return true;
}

std::shared_ptr<MemoryManager> GGetActiveMemoryManager()
{
	if (GActiveMemoryManager == nullptr)
	{
		spdlog::info("Memory Manager is Invalid/Null");
		return nullptr;
	}
    return GActiveMemoryManager;
}