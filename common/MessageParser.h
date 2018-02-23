//---------------------------------------------------------------
//
// MessageParser.cpp
//

#pragma once

#include <memory>
#include <vector>
#include <string>

namespace Common {

//===============================================================================
struct NetworkMessage;
struct MessageHeader;
class MessageParser
{
public:
	MessageParser();
	~MessageParser();

	// Returns true if any messages are parsed successfully.
	bool ParseMessage(const std::string& stream, std::vector<NetworkMessage>& messages);

private:
	// Swaps the currently active buffer.
	void SwapBuffer();
	void SetDataOnActiveBuffer(const char* data, int size);

private:
	// Holds data about the content we're trying to receive. Header data will be copied three times.
	// Content is only copied twice. Once to our buffers, and once to the final output.
	std::unique_ptr<MessageHeader> m_header = std::make_unique<MessageHeader>();

	// Buffers are swapped between after successful header or content reads.
	std::string m_firstBuffer;
	std::string m_secondBuffer;

	// Points to the buffer currently being filled with data.
	std::string* m_activeBuffer;

	// We should be looking for header data if this is false.
	bool m_isHeaderSet = false;

	// Total number of bytes received on this message across parse attempts.
	uint32_t m_totalBytesParsed = 0;
};

//===============================================================================

} // namespace Common
