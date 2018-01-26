//---------------------------------------------------------------
//
// ByteBuffer.h
//

#pragma once

#include "ByteBuffer.h"

#include <memory>

namespace Common {

//===============================================================================

enum struct MessageType : uint32_t
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
		messageType = MessageType::None;
		messageLength = 0;
	};

	MessageType messageType = MessageType::None;
	uint32_t messageLength = 0;
};

struct NetworkMessage
{
	NetworkMessage() {}
	NetworkMessage(const MessageHeader& h)
		: header(h)
		, messageData(header.messageLength)
	{
	}

	MessageHeader header;
	ByteBuffer messageData;
};

//===============================================================================

} // namespace Common
