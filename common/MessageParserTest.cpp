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
	const char s_messageData[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n' };
	const int s_bufferSize = 50;
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
	//----------------------------------------------------
	// Data Setup
	//----------------------------------------------------
	MessageHeader header;
	header.messageType = MessageType::Attack;
	header.messageLength = 14;
	
	auto ClearBuffer = [](uint8_t buffer[], int& size)
	{
		for (int i = 0; i < size; ++i)
		{
			buffer[i] = 0;
		}
		size = 0;
	};

	int size = s_bufferSize;
	std::unique_ptr<uint8_t[]> netBuffer = std::make_unique<uint8_t[]>(50);
	ClearBuffer(netBuffer.get(), size);

	//----------------------------------------------------

	// Test 1)
	// Receive single message in one chunk
	NetworkMessage msg;
	assert(sizeof(header) == 8);
	std::memcpy(netBuffer.get(), &header, 8);
	std::memcpy(netBuffer.get() + 8, s_messageData, 14);
	m_parser->ParseMessage(netBuffer.get(), sizeof(header) + header.messageLength, msg);
	Verify(msg);


	// Test 2)
	// Receive the header
	// Receive remainder of message in 3 chunks

	// Test 3)
	// Receive the header
	// Receive remainder of message in 3 chunks
	// Last chunk has part of next message
	// Receive rest of message in next chunk


	// Test 4)
	// Receive the header
	// Receive remainder of message in 3 chunks
	// Last chunk has last chunk + entire next message
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
	LOG_DEBUG("PASS: Single message parser test succeeded.");
}

//===============================================================================

} // namespace Test
} // namespace Common
