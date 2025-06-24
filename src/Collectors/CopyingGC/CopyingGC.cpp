#include "CopyingGC.h"
#include "../../Allocators/FreeList/FreeList.h"

std::shared_ptr<GCStats> CopyingGC::collect() {
	_resetStats();
	copy();
	forward();
	fixPointer();
	swap();
	return stats;
}

void CopyingGC::copy()
{
	std::vector<Word> roots = _roots;

	auto dstAlloc = 0;

	while (!roots.empty()) {
		Word v = roots.back();
		roots.pop_back();
		ObjectHeader* header = FromAllocator->getHeader(v);

		stats->alive++;

		auto newLoc = CopyToNewHeap(v - sizeof(ObjectHeader), FromAllocator, dstAlloc, ToAllocator);
		ReplaceHeaderWithRawPtr(v, FromAllocator, newLoc);
		//for (const auto& p : ToAllocator->getPointers(v))
		//{
		//	if (!isObjectHeaderRawPointer((Word*)FromAllocator->getHeader(*p)) && FromAllocator->getHeader(*p)->size != 0)
		//	{
		//		roots.push_back(p->decode());
		//	}
		//}
		dstAlloc += sizeof(ObjectHeader) + header->size;
	}
}

void CopyingGC::forward()
{
}

void CopyingGC::fixPointer()
{
}

void CopyingGC::swap()
{
}
