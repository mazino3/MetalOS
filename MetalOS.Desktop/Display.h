#pragma once
#pragma once
#include <cstdint>
#include <MetalOS.h>
#include <Debug.h>

class Display
{
public:
	Display(const size_t height, const size_t width) :
		m_height(height),
		m_width(width),
		m_backing(),
		is_init(false)
	{
		
	}

	void Initialize()
	{
		const size_t size = m_width * m_height * sizeof(Color);
		m_backing = (Color*)VirtualAlloc(nullptr, size, MemoryAllocationType::Commit, MemoryProtection::PageReadWrite);
		Assert(m_backing);

		is_init = true;
	}

	void Set(Point2D p, Color value)
	{
		const size_t index = GetIndex(p);
		m_backing[index] = value;
	}

	Color Get(Point2D p) const
	{
		const size_t index = GetIndex(p);
		return m_backing[index];
	}

	size_t GetHeight() const
	{
		return m_height;
	}

	size_t GetWidth() const
	{
		return m_width;
	}

	Color* Buffer() const
	{
		return m_backing;
	}

	size_t Size()
	{
		return m_height * m_width * sizeof(Color);
	}

private:
	constexpr size_t GetIndex(Point2D p) const
	{
		Assert(p.Y < m_height);
		Assert(p.X < m_width);
		return p.Y * m_width + p.X;
	}

	size_t m_height;
	size_t m_width;
	Color* m_backing;
	bool is_init;
};
