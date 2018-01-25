//---------------------------------------------------------------
//
// MessageParser.cpp
//

#include "MessageParser.h"

namespace Common {

//===============================================================================

namespace
{
	const uint32_t s_headerSize = 8;
}

MessageParser::MessageParser()
	: m_activeBuffer(&m_firstBuffer)
{
}

MessageParser::~MessageParser()
{

}

void MessageParser::ParseMessage(const uint8_t rawData[], uint32_t size, NetworkMessage& message)
{
	BuildHeader(rawData, size);

	// Attempt to set the message from our data.
	BuildMessage(rawData, size, message);
}

void MessageParser::BuildHeader(const uint8_t rawData[], uint32_t& size)
{
	if (m_header.messageLength == 0)
	{
		int overflow = m_bytesReceivedThisMessage + size - s_headerSize;
		m_bytesReceivedThisMessage += size;
		if (overflow >= 0)
		{
			// Accumulate header data.
			int headerRemainder = size - overflow;
			m_activeBuffer->SetData(rawData, headerRemainder);

			memcpy(&m_header, m_activeBuffer->GetData(), s_headerSize);
			// Put overflow data in next buffer.
			SwapBuffer();
			m_activeBuffer->SetData(rawData + headerRemainder, overflow);

			// We have already handled the header in our old buffer. We shouldn't consider it
			// while parsing the message.
			size -= s_headerSize;
		}
		else
		{
			m_activeBuffer->SetData(rawData, size);
		}
	}
}

void MessageParser::BuildMessage(const uint8_t rawData[], uint32_t size, NetworkMessage& message)
{
	if (m_header.messageLength != 0)
	{
		// Calculate number of overflow bytes
		// If we had 3 bytes already, needed 5, but received 12, 10 bytes would be overflowing.
		int overflow = m_activeBuffer->GetSize() - size;

		if (overflow >= 0)
		{
			// Copy remainder of the message to our active buffer.
			int messageRemainder = size - overflow;

			// Copy full message into our message struct.
			message.messageData->SetData(m_activeBuffer->GetData(), m_header.messageLength);
			message.header = m_header;

			// Put the overflow into next buffer.
			SwapBuffer();
			m_activeBuffer->SetData(rawData + messageRemainder, overflow);

			m_header.Clear();
			m_bytesReceivedThisMessage = 0;
		}
		else
		{
			m_activeBuffer->SetData(rawData, size);
		}
	}
}

void MessageParser::SwapBuffer()
{
	if (m_activeBuffer == &m_firstBuffer)
	{
		m_activeBuffer = &m_secondBuffer;
		m_firstBuffer.ClearData();
	}
	else
	{
		m_activeBuffer = &m_firstBuffer;
		m_secondBuffer.ClearData();
	}
}

void MessageParser::ClearInactiveBuffer()
{
	if (m_activeBuffer == &m_firstBuffer)
	{
		m_secondBuffer.ClearData();
	}
	else
	{
		m_firstBuffer.ClearData();
	}
}

//===============================================================================

} // namespace Common
