#pragma once

#include <cassert>

#include "och_range.h"

template<typename T>
struct heap_buffer
{
private:

	och::range<T> m_range;

public:

	explicit heap_buffer() noexcept : m_range{ nullptr, nullptr } {}

	explicit heap_buffer(uint32_t cnt) noexcept : m_range{ static_cast<T*>(malloc(cnt * sizeof(T))), cnt } {}

	explicit heap_buffer(och::range<T> range) noexcept : m_range{ range } {}

	~heap_buffer() noexcept
	{
		deallocate();
	}

	T* data() noexcept
	{
		return m_range.beg;
	}

	const T* data()	const noexcept
	{
		return m_range.beg;
	}

	T& operator[](uint32_t n) noexcept
	{
		assert(n < m_range.len());

		return m_range[n];
	}

	const T& operator[](uint32_t n) const noexcept
	{
		assert(n < m_range.len());

		return m_range[n];
	}

	void allocate(uint32_t cnt) noexcept
	{
		if (m_range.beg)
			free(m_range.beg);

		m_range.beg = static_cast<T*>(malloc(cnt * sizeof(T)));

		m_range.end = m_range.beg + cnt;
	}

	void deallocate() noexcept
	{
		if (m_range.beg)
			free(m_range.beg);

		m_range.beg = m_range.end = nullptr;
	}

	och::range<T> detach() noexcept
	{
		och::range<T> tmp = m_range;

		m_range.beg = m_range.end = nullptr;

		return tmp;
	}

	void attach(och::range<T> range)
	{
		if (m_range.beg)
			free(m_range.beg);

		m_range = range;
	}

	uint32_t size() const noexcept
	{
		return static_cast<uint32_t>(m_range.len());
	}

	T* begin() noexcept
	{
		return m_range.beg;
	}

	const T* begin() const noexcept
	{
		return m_range.beg;
	}

	T* end() noexcept
	{
		return m_range.end;
	}

	const T* end() const noexcept
	{
		return m_range.end;
	}

	och::range<T> range() noexcept
	{
		return m_range;
	}

	const och::range<T> range() const noexcept
	{
		return m_range;
	}

	void shrink(uint32_t new_size) noexcept
	{
		m_range.end = m_range.beg + new_size;
	}
};
