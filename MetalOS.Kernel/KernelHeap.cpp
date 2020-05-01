#include "KernelHeap.h"

#include "Main.h"

KernelHeap::KernelHeap(uintptr_t address, uint32_t size) : m_address(address), m_size(size), m_allocated(0), m_head((PHEAP_BLOCK)address)
{
	//Initialize by creating a head free block
	this->m_head->Size = size;
	this->m_head->Flags = 0;
	this->m_head->Free = true;
	this->m_head->Next = nullptr;
	this->m_head->Magic = Magic;
}

//TODO: take into account alignment
uintptr_t KernelHeap::Allocate(uint32_t size)
{
	PHEAP_BLOCK pBlock = this->m_head;
	while (!pBlock->Free || pBlock->Size < size + sizeof(HEAP_BLOCK))
	{
		//End of the list, no space
		if (pBlock->Next == nullptr)
		{
			Assert(false);
			return 0;
		}

		pBlock = pBlock->Next;
	}

	//Update block
	pBlock->Free = false;

	//Update statistics
	this->m_allocated += size;

	uintptr_t address = (uintptr_t)&pBlock->Block;
	if (pBlock->Size - (size + sizeof(HEAP_BLOCK)) < KernelHeap::MinBlockSize + sizeof(HEAP_BLOCK))
	{
		//Not enough size to split, just return
		return address;
	}
	
	//Split the block
	PHEAP_BLOCK newBlock = (PHEAP_BLOCK)(address + size);
	newBlock->Size = pBlock->Size - size - sizeof(HEAP_BLOCK);
	newBlock->Flags = 0;
	newBlock->Free = 1;
	newBlock->Magic = Magic;
	newBlock->Next = pBlock->Next;

	//Update current block
	pBlock->Size = size;
	pBlock->Next = newBlock;

	return address;
}

void KernelHeap::Deallocate(uintptr_t address)
{
	Assert(address >= this->m_address + sizeof(HEAP_BLOCK));
	Assert(address < this->m_address + this->m_size);

	PHEAP_BLOCK pBlock = (PHEAP_BLOCK)(address - sizeof(HEAP_BLOCK));
	Assert(pBlock->Magic == Magic);
	
	pBlock->Free = true;

	//TODO: check flags?
	//TODO: condense with block before? change to double pointer blocks?
	if (pBlock->Next->Free)
	{
		//Combine this and next blocks
		pBlock->Size += pBlock->Next->Size + sizeof(HEAP_BLOCK);
		pBlock->Next = pBlock->Next->Next;
	}
}

void KernelHeap::PrintHeap()
{
	PHEAP_BLOCK current = this->m_head;
	while (current != nullptr)
	{
		Print("  P 0x%16x S: 0x%8x F: 0x%8x N: 0x%16x", current, current->Size, current->Flags, current->Next);
		current = current->Next;
	}
}
