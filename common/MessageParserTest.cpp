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
	, m_helper(this)
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

void MessageParserTest::ClearBuffer(uint8_t buffer[])
{
	for (int i = 0; i < s_bufferSize; ++i)
	{
		buffer[i] = 0;
	}
}

void MessageParserTest::Verify(const NetworkMessage& message)
{
	assert(message.header.messageLength == 14);
	assert(message.header.messageType == MessageType::Attack);
	assert(message.messageData.GetSize() == 14);
	for (int i = 0; i < 14; ++i)
	{
		assert(message.messageData.GetData()[i] == s_messageData[i]);
	}
}

void MessageParserTest::TestSingle()
{
	// Test 1)
	// Send full message in a single chunk.

	std::unique_ptr<uint8_t[]> netBuffer = std::make_unique<uint8_t[]>(s_bufferSize);

	assert(sizeof(s_header) == 8);

	std::memcpy(netBuffer.get(), &s_header, 8);
	std::memcpy(netBuffer.get() + 8, s_messageData, 14);
	bool messageFilled = false;

	std::vector<NetworkMessage> messages;
	messageFilled = m_parser->ParseMessage(netBuffer.get(), sizeof(s_header) + s_header.messageLength, messages);
	assert(messageFilled);
	Verify(messages[0]);

	LOG_DEBUG("PASS: Single message parse test succeeded.");
}

void MessageParserTest::TestMulti()
{
	// Test 4)
	// Send 4 messages in one chunk.
}

void MessageParserTest::TestPartial()
{
	// Test 2)
	// Send the header
	// Send remainder of message in 3 chunks
	// - 3 bytes, 6 bytes, 5 bytes.
	std::unique_ptr<uint8_t[]> netBuffer = std::make_unique<uint8_t[]>(s_bufferSize);

	std::vector<NetworkMessage> messages;
	bool hasMessages = false;

	// Send header.
	m_helper.SendPartialHeader(netBuffer.get(), 8, messages, hasMessages);
	assert(!hasMessages);
	m_helper.Clear();

	// Send a chunk.
	m_helper.SendPartialMessage(netBuffer.get(), 3, messages, hasMessages);
	assert(!hasMessages);

	m_helper.SendPartialMessage(netBuffer.get(), 6, messages, hasMessages);
	assert(!hasMessages);
	
	m_helper.SendPartialMessage(netBuffer.get(), 5, messages, hasMessages);
	assert(hasMessages);

	Verify(messages[0]);
	LOG_DEBUG("PASS: Partial data message parse test succeeded.");
	messages.pop_back();

	// Test 6)
	// Send header in 4 chunks: 2 bytes, 2 bytes, 3 bytes, 1 byte.
	// Send message in 2, 7 byte chunks.
}

void MessageParserTest::TestUneven()
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

//--------------------------------------------------------------------------------
MessageParserTest::MessageHelper::MessageHelper(MessageParserTest* tester)
	: m_tester(tester)
{
}

void MessageParserTest::MessageHelper::SendPartialMessage(uint8_t data[], int sizeToSend,
	std::vector<NetworkMessage>& messages, bool& hasMessages)
{
	std::memcpy(data, s_messageData + m_sizeSent, sizeToSend);
	m_sizeSent += sizeToSend;
	SendPartialData(data, sizeToSend, messages, hasMessages);
}

void MessageParserTest::MessageHelper::SendPartialHeader(uint8_t data[], int sizeToSend,
	std::vector<NetworkMessage>& messages, bool& hasMessages)
{
	std::memcpy(data, &s_header, sizeToSend);
	m_sizeSent += sizeToSend;
	SendPartialData(data, sizeToSend, messages, hasMessages);
}

void MessageParserTest::MessageHelper::Clear()
{
	m_sizeSent = 0;
}

void MessageParserTest::MessageHelper::SendPartialData(uint8_t data[], int sizeToSend,
	std::vector<NetworkMessage>& messages, bool& hasMessages)
{
	hasMessages = m_tester->m_parser->ParseMessage(data, sizeToSend, messages);
	m_tester->ClearBuffer(data);
}

//===============================================================================

} // namespace Test
} // namespace Common
