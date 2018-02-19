//---------------------------------------------------------------
//
// ByteBuffer.cpp
//

#include "ByteBuffer.h"
#include <iterator>
#include <numeric>
#include <algorithm>

namespace Common {

//===============================================================================

namespace {
	const int s_defaultBufferSize = 64;
}

ByteBuffer::ByteBuffer()
	: m_bufferSize(s_defaultBufferSize)
{
}

ByteBuffer::ByteBuffer(ByteBuffer&& b) noexcept
	: m_heapBuffer(std::move(b.m_heapBuffer))
	, m_size(std::move(b.m_size))
	, m_maxSinceClear(std::move(b.m_maxSinceClear))
{

	m_activeBuffer = m_size > s_defaultBufferSize ? m_heapBuffer.data() : &m_stackBuffer[0];
	if (IsUsingStackBuffer())
	{
		for (uint8_t i = 0; i < b.m_size; ++i)
		{
			m_stackBuffer[i] = b.m_stackBuffer[i];
		}
	}
}

ByteBuffer::~ByteBuffer()
{
}

const uint8_t* ByteBuffer::GetData() const
{
	return m_activeBuffer;
}

void ByteBuffer::SetData(const uint8_t data[], uint8_t size)
{
	int newSize = size + m_size;
	if (newSize <= s_defaultBufferSize)
	{
		CopyDataToBuffer(data, size);
	}
	else
	{
		ExpandBuffer(newSize);
		CopyDataToBuffer(data, size);
	}
}

void ByteBuffer::ClearData()
{
	for (uint32_t i = 0; i < m_maxSinceClear; ++i)
	{
		m_activeBuffer[i] = 0;
	}
	m_size = 0;
	m_maxSinceClear = 0;

	m_heapBuffer.clear();

	// We just cleared data, go back to using stack memory unless we must use heap.
	m_activeBuffer = &m_stackBuffer[0];
}

void ByteBuffer::ExpandBuffer(int newSize)
{
	if (newSize >= m_heapBuffer.size())
	{
		int resizeTo = m_heapBuffer.size() + s_defaultBufferSize;
		while (resizeTo < newSize)
		{
			resizeTo += s_defaultBufferSize;
		}

		// Resize to fit the new contents in 64 byte chunks.
		m_heapBuffer.resize(resizeTo);
	}

	if (IsUsingStackBuffer())
	{
		// Switch to heap.
		m_activeBuffer = m_heapBuffer.data();
		if (m_size > 0)
		{
			std::memcpy(m_activeBuffer, m_stackBuffer, m_size);
		}
	}
}

void ByteBuffer::CopyDataToBuffer(const uint8_t data[], uint8_t size)
{
	std::memcpy(m_activeBuffer + m_size, data, size);
	m_size += size;
	if (m_size > m_maxSinceClear)
	{
		m_maxSinceClear = m_size;
	}
}

bool ByteBuffer::IsUsingStackBuffer() const
{
	return m_activeBuffer == &m_stackBuffer[0];
}

//===============================================================================

} // namespace Common
