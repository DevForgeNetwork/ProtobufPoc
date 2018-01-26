//---------------------------------------------------------------
//
// MessageParser.cpp
//

#include "MessageParser.h"
#include <algorithm>

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

bool MessageParser::ParseMessage(const uint8_t rawData[], uint32_t size, std::vector<NetworkMessage>& messages)
{
	int headerOffset = 0;
	while (m_totalBytesParsed < size)
	{
		if (!m_isHeaderSet)
		{
			int startSize = m_activeBuffer->GetSize();

			// Copy up to the full header data into the current buffer.
			m_activeBuffer->SetData(rawData, std::min(size, s_headerSize - startSize));
			m_totalBytesParsed += m_activeBuffer->GetSize();

			// If we have a full header.
			if (m_activeBuffer->GetSize() == s_headerSize)
			{
				memcpy(&m_header, m_activeBuffer->GetData(), sizeof(m_header));
				m_isHeaderSet = true;

				// If this set of raw data contains header data, note the offset for our message.
				headerOffset = s_headerSize - startSize;

				// Message data goes into the next buffer.
				SwapBuffer();
			}
		}
		if (m_isHeaderSet && m_totalBytesParsed < size)
		{
			int sizeToCopy = std::min(size, m_header.messageLength - m_activeBuffer->GetSize());

			// Copy up to full message data into current buffer.
			m_activeBuffer->SetData(rawData + headerOffset, sizeToCopy);
			m_totalBytesParsed += sizeToCopy;

			// If we have a full message.
			if (m_activeBuffer->GetSize() == m_header.messageLength)
			{
				// Package data into NetworkMessage and copy to out-data.
				NetworkMessage msg(m_header);
				msg.messageData.SetData(m_activeBuffer->GetData(), m_header.messageLength);
				messages.push_back(std::move(msg));

				// Reset state.
				m_isHeaderSet = false;
				m_header.Clear();
				SwapBuffer();
			}
		}
	}

	m_totalBytesParsed = 0;
	return !messages.empty();
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

//===============================================================================

} // namespace Common
