//---------------------------------------------------------------
//
// ByteBuffer.h
//

#pragma once

#include <memory>
#include <string>

namespace Common {

//===============================================================================

enum struct MessageId : uint32_t
{
	  None
	, Creature
	, Attack
	, Move
};

struct MessageHeader
{
	void Clear()
	{
		messageType = MessageId::None;
		messageLength = 0;
	};

	MessageId messageType = MessageId::None;
	uint32_t messageLength = 0;
};

struct NetworkMessage
{
	NetworkMessage() {}
	NetworkMessage(NetworkMessage&& m)
		: header(std::move(m.header))
		, messageData(std::move(m.messageData))
	{
	}

	NetworkMessage(const MessageHeader& h)
		: header(h)
	{
	}

	MessageHeader header;
	std::string messageData;
};



//===============================================================================

} // namespace Common
