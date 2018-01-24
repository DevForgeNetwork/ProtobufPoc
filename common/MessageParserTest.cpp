//---------------------------------------------------------------
//
// MessageParserTest.cpp
//

#include "MessageParserTest.h"

#include "MessageParser.h"
#include "NetworkTypes.h"

#include <assert.h>

namespace Common {
namespace Test {

//===============================================================================

MessageParserTest::MessageParserTest()
{
}

MessageParserTest::~MessageParserTest()
{
}

void MessageParserTest::RunTest()
{
	//----------------------------------------------------
	// Data Setup
	//----------------------------------------------------
	MessageHeader header;
	header.messageLength = 14;
	char messageData[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n' };
	auto ClearBuffer = [](uint8_t buffer[], int& size)
	{
		for (int i = 0; i < size; ++i)
		{
			buffer[i] = 0;
		}
		size = 0;
	};

	int size = 50;
	std::unique_ptr<uint8_t[]> netBuffer = std::make_unique<uint8_t[]>(50);
	ClearBuffer(netBuffer.get(), size);

	//----------------------------------------------------

	// Test 1)
	// Receive single message in one chunk
	NetworkMessage msg;
	assert(sizeof(header) == 8);
	std::memcpy(netBuffer.get(), &header, 8);
	std::memcpy(netBuffer.get() + 8, messageData, 14);
	m_parser->ParseMessage(netBuffer.get(), sizeof(header) + header.messageLength, msg);

	assert(msg.header.messageLength == 14);
	assert(msg.header.messageType == MessageType::Creature);
	assert(msg.messageData->GetSize() == 14);
	for (int i = 0; i < 14; ++i)
	{
		assert(msg.messageData->GetData()[i] == messageData[i]);
	}

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

//===============================================================================

} // namespace Test
} // namespace Common
