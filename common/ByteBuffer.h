//---------------------------------------------------------------
//
// ByteBuffer.h
//

#pragma once

#include <memory>

namespace Common {

//===============================================================================

class ByteBuffer
{
public:
	ByteBuffer(int sizeOverride = -1);
	ByteBuffer(ByteBuffer&& b) noexcept
		: m_data(std::move(b.m_data))
		, m_size(std::move(b.m_size))
		, m_maxSinceClear(std::move(b.m_maxSinceClear))
	{

	}
	~ByteBuffer();

	// Returns raw byte data.
	uint8_t* GetData() const { return m_data.get(); }

	// Gets the the number of bytes.
	uint8_t GetSize() const { return m_size; }

	// Sets the raw data and increments the size.
	void SetData(const uint8_t data[], uint8_t size);

	// Clear raw data and reset size.
	void ClearData();

private:
	void InitializeData();

private:
	// How many bytes of data we currently hold.
	uint32_t m_size = 0;

	// Size of the entire buffer.
	uint32_t m_bufferSize = 0;

	std::unique_ptr<uint8_t[]> m_data;

	// Holds the biggest we've been since we've last been cleared. This is just an optimization.
	// This way we don't need to clear the entire buffer, just up to our highest index.
	uint32_t m_maxSinceClear;
};

//===============================================================================

} //namespace Common
