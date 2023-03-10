#include "Kernel.h"
#include "Assert.h"

#include "BootHeap.h"

BootHeap::BootHeap(const void* address, const size_t length) :
	m_address((uintptr_t)address),
	m_length(length),
	m_watermark(m_address)
{

}

void* BootHeap::Allocate(const size_t size)
{
	Assert(m_watermark + size <= m_address + m_length);

	const uintptr_t level = m_watermark;
	m_watermark += ByteAlign(size, Alignment);
	return (void*)level;
}

//Don't deallocate.
//Some temporary objects will get instantiated during boot (std::string).
void BootHeap::Deallocate(void* const address)
{

}
