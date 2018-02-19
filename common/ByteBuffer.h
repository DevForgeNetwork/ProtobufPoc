//---------------------------------------------------------------
//
// ByteBuffer.h
//

#pragma once

#include <memory>
#include <vector>
#include <algorithm>

namespace Common {

//===============================================================================

class ByteBuffer
{
public:
	ByteBuffer();
	ByteBuffer(ByteBuffer&& b) noexcept;
	
	~ByteBuffer();

	// Returns raw byte data.
	const uint8_t* GetData() const;

	// Gets the the number of bytes.
	uint8_t GetSize() const { return m_size; }

	// Sets the raw data and increments the size.
	void SetData(const uint8_t data[], uint8_t size);

	// Clear raw data and reset size.
	void ClearData();

private:
	// Buffer size has grown beyond our stack allocation. Allocate heap space and move current buffer to it.
	void ExpandBuffer(int newSize);

	// Copy data to the active buffer.
	void CopyDataToBuffer(const uint8_t data[], uint8_t size);

	// Returns true if the stack buffer is the active buffer, false if we're using the heap.
	bool IsUsingStackBuffer() const;

private:
	// How many bytes of data we currently hold.
	uint32_t m_size = 0;

	// Small buffer optimization. Will have 64 bytes on the stack. If we need more we'll move to the heap.
	uint8_t m_stackBuffer[64] = { 0 };

	// Heap will grow to whatever size is needed, in 64 byte chunks.
	std::vector<uint8_t> m_heapBuffer;

	// Pointer to whichever buffer we're currently using.
	uint8_t* m_activeBuffer = &m_stackBuffer[0];

	// Holds the biggest we've been since we've last been cleared. This is just an optimization.
	// This way we don't need to clear the entire buffer, just up to our highest index.
	uint32_t m_maxSinceClear = 0;
};

//===============================================================================

} //namespace Common
