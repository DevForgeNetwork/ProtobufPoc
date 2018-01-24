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
	int messageLength = 0;
};

struct NetworkMessage
{
	MessageHeader header;
	std::unique_ptr<ByteBuffer> messageData;
};

//===============================================================================

} // namespace Common
