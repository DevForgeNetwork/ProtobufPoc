//---------------------------------------------------------------
//
// MessageParserTest.cpp
//

#include "MessageParserTest.h"

#include "ByteBuffer.h"
#include "Log.h"
#include "MessageParser.h"
#include "NetworkTypes.h"


#include <assert.h>

namespace Common {
namespace Test {

//===============================================================================

namespace
{
	const MessageHeader s_header{ MessageType::Attack, 14 };
	const char s_messageData[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n' };
	const int s_bufferSize = 100;
}

MessageParserTest::MessageParserTest()
	: m_parser(std::make_unique<Common::MessageParser>())
{
}

MessageParserTest::~MessageParserTest()
{
}


void MessageParserTest::RunTests()
{
	TestSingle();
	TestPartial();
	TestUneven();
	TestMulti();
}

void MessageParserTest::ClearBuffer(uint8_t buffer[]) const
{
	for (int i = 0; i < s_bufferSize; ++i)
	{
		buffer[i] = 0;
	}
}

void MessageParserTest::Verify(const NetworkMessage& message) const
{
	assert(message.header.messageLength == 14);
	assert(message.header.messageType == MessageType::Attack);
	assert(message.messageData.GetSize() == 14);
	for (int i = 0; i < 14; ++i)
	{
		assert(message.messageData.GetData()[i] == s_messageData[i]);
	}
}

void MessageParserTest::TestSingle() const
{
	// Test 1)
	// Send full message in a single chunk.

	std::unique_ptr<uint8_t[]> netBuffer = std::make_unique<uint8_t[]>(s_bufferSize);

	assert(sizeof(s_header) == 8);

	std::memcpy(netBuffer.get(), &s_header, 8);
	std::memcpy(netBuffer.get() + 8, s_messageData, 14);
	bool messageFilled = false;

	std::vector<NetworkMessage> msg;
	messageFilled = m_parser->ParseMessage(netBuffer.get(), sizeof(s_header) + s_header.messageLength, msg);
	assert(messageFilled);
	Verify(msg[0]);

	LOG_DEBUG("PASS: Single message parse test succeeded.");
}

void MessageParserTest::TestMulti() const
{
	// Test 4)
	// Send 4 messages in one chunk.
}

void MessageParserTest::TestPartial() const
{
	// Test 2)
	// Send the header
	// Send remainder of message in 3 chunks
	// - 3 bytes, 6 bytes, 5 bytes.
	std::unique_ptr<uint8_t[]> netBuffer = std::make_unique<uint8_t[]>(s_bufferSize);

	std::vector<NetworkMessage> msg;
	bool messageFilled = false;

	// Send header.
	std::memcpy(netBuffer.get(), &s_header, 8);
	messageFilled = m_parser->ParseMessage(netBuffer.get(), sizeof(s_header), msg);
	assert(!messageFilled);

	int sizeSent = 0;
	int sizeToSend = 3;

	// Send a chunk.
	std::memcpy(netBuffer.get(), s_messageData + sizeSent, sizeToSend);
	sizeSent += sizeToSend;
	messageFilled = m_parser->ParseMessage(netBuffer.get(), sizeToSend, msg);
	assert(!messageFilled);
	ClearBuffer(netBuffer.get());

	sizeToSend = 6;
	std::memcpy(netBuffer.get(), s_messageData + sizeSent, sizeToSend);
	sizeSent += sizeToSend;
	messageFilled = m_parser->ParseMessage(netBuffer.get(), sizeToSend, msg);
	assert(!messageFilled);
	ClearBuffer(netBuffer.get());

	sizeToSend = 5;
	std::memcpy(netBuffer.get(), s_messageData + sizeSent, sizeToSend);
	sizeSent += sizeToSend;
	messageFilled = m_parser->ParseMessage(netBuffer.get(), sizeToSend, msg);
	assert(messageFilled);
	ClearBuffer(netBuffer.get());
	Verify(msg[0]);

	LOG_DEBUG("PASS: Partial data message parse test succeeded.");

	// Test 6)
	// Send header in 4 chunks: 2 bytes, 2 bytes, 3 bytes, 1 byte.
	// Send message in 2, 7 byte chunks.
}

void MessageParserTest::TestUneven() const
{
	// Test 3)
	// Send the header.
	// Send remainder of message in 3 chunks.
	// Last chunk has first 2 bytes of next message header.
	// Send rest of data in one chunk.

	// Test 5)
	// Send the header.
	// Send remainder of message in 3 chunks.
	// Last chunk has last chunk + entire next message.
}

//===============================================================================

} // namespace Test
} // namespace Common
