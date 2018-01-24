//---------------------------------------------------------------
//
// MessageParser.cpp
//
#include <memory>

#pragma once

#include "NetworkTypes.h"

namespace Common {

//===============================================================================

class MessageParser
{
public:
	MessageParser();
	~MessageParser();
	void ParseMessage(const uint8_t rawData[], uint32_t size, NetworkMessage& message);

private:
	// Reads in data for the message header.
	void BuildHeader(const uint8_t rawData[], uint32_t& size);

	// Reads in data for the content of the message.
	void BuildMessage(const uint8_t rawData[], uint32_t size, NetworkMessage& msg);

	// Swaps the currently active buffer.
	void SwapBuffer();

	// Clears the opposite buffer to the active one.
	void ClearInactiveBuffer();

private:
	// Holds data about the content we're trying to receive. Header data will be copied three times.
	// Content is only copied twice. Once to our buffers, and once to the final output.
	MessageHeader m_header;

	// Buffers are swapped between after successful header or content reads.
	ByteBuffer m_firstBuffer;
	ByteBuffer m_secondBuffer;

	// Points to the buffer currently being filled with data.
	ByteBuffer* m_activeBuffer;

	// Total number of bytes received on this message across parse attempts.
	uint32_t m_bytesReceivedThisMessage = 0;
};

//===============================================================================

} // namespace Common
