//---------------------------------------------------------------
//
// ByteBuffer.cpp
//

#include "ByteBuffer.h"

namespace Common {

//===============================================================================

namespace {
	const uint32_t s_bufferSize = 64000;
}

ByteBuffer::ByteBuffer()
	: m_data(std::make_unique<uint8_t[]>(s_bufferSize))
{
	InitializeData();
}

ByteBuffer::~ByteBuffer()
{
}

void ByteBuffer::SetData(const uint8_t data[], uint8_t size)
{
	if (m_size + size < s_bufferSize)
	{
		std::memcpy(m_data.get() + m_size, data, size);
		m_size += size;
		if (m_size > m_maxSinceClear)
		{
			m_maxSinceClear = m_size;
		}
	}
}

void ByteBuffer::ClearData()
{
	for (uint32_t i = 0; i < m_maxSinceClear; ++i)
	{
		m_data[i] = 0;
	}
	m_size = 0;
	m_maxSinceClear = 0;
}

void ByteBuffer::InitializeData()
{
	for (int i = 0; i < s_bufferSize; ++i)
	{
		m_data[i] = 0;
	}
}

//===============================================================================

} // namespace Common
