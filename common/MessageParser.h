//---------------------------------------------------------------
//
// MessageParser.cpp
//

#pragma once

#include "NetworkTypes.h"
#include <memory>
#include <vector>

namespace Common {

//===============================================================================

class MessageParser
{
public:
	MessageParser();
	~MessageParser();

	// Returns true if any messages are parsed successfully.
	bool ParseMessage(const uint8_t rawData[], uint32_t size, std::vector<NetworkMessage>& messages);

private:
	// Swaps the currently active buffer.
	void SwapBuffer();

private:
	// Holds data about the content we're trying to receive. Header data will be copied three times.
	// Content is only copied twice. Once to our buffers, and once to the final output.
	MessageHeader m_header;

	// Buffers are swapped between after successful header or content reads.
	ByteBuffer m_firstBuffer;
	ByteBuffer m_secondBuffer;

	// Points to the buffer currently being filled with data.
	ByteBuffer* m_activeBuffer;

	// We should be looking for header data if this is false.
	bool m_isHeaderSet = false;

	// Total number of bytes received on this message across parse attempts.
	uint32_t m_totalBytesParsed = 0;
};

//===============================================================================

} // namespace Common
