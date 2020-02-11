#pragma once
#include "MemoryMap.h"
#include "Bitvector.h"

//https://wiki.osdev.org/Page_Frame_Allocation
//TODO: make this take page_size in case usage changes to allocate in groups of N pages
class PageFrameAllocator
{
public:
	PageFrameAllocator(MemoryMap& memoryMap);

	uintptr_t AllocatePage();
	void* AllocatePages(size_t count);

	void DeallocatePage(uintptr_t address);
	void DeallocatePages(uintptr_t address, size_t count);

private:
	size_t m_nextIndex;
	Bitvector m_bitmap;
	MemoryMap& m_memoryMap;
};
