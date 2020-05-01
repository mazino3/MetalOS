#pragma once
#include <cstdint>
#include <MetalOS.h>
#include "Main.h"

//How doesn't C++ have a way to do contiguous multi-dimensional arrays?
template<class T>
class Matrix
{
public:
	Matrix(size_t M, size_t N) : m_height(M), m_width(N), m_backing(new T[N * M * sizeof(T)])
	{
		Assert(m_backing);
	}

	void Set(Point2D p, T value)
	{
		const size_t index = GetIndex(p);
		m_backing[index] = value;
	}

	T Get(Point2D p) const
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

private:
	constexpr size_t GetIndex(Point2D p) const
	{
		Assert(p.Y < m_height);
		Assert(p.X < m_width);
		return p.Y * m_width + p.X;
	}

	size_t m_height;
	size_t m_width;
	T* m_backing;
};
