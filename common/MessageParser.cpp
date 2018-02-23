//---------------------------------------------------------------
//
// MessageParser.cpp
//

#include "MessageParser.h"

#include "NetworkTypes.h"

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

bool MessageParser::ParseMessage(const std::string& data, std::vector<NetworkMessage>& messages)
{
	int headerOffset = 0;
	int messageOffset = 0;
	while (m_totalBytesParsed < data.size())
	{
		if (!m_isHeaderSet)
		{
			int startSize = m_activeBuffer->size();

			// Copy up to the full header data into the current buffer. This header could start after
			// the end of a previous message, so we need to account for that offset. We also need to account
			// for the amount of bytes we've parse so far, so we copy the correct amount of header data.
			SetDataOnActiveBuffer(&data[0] + messageOffset, std::min(data.size() - m_totalBytesParsed, s_headerSize - startSize));
			m_totalBytesParsed += m_activeBuffer->size();

			// If we have a full header.
			if (m_activeBuffer->size() == s_headerSize)
			{
				std::memcpy(m_header.get(), &(*m_activeBuffer)[0], sizeof(*m_header.get()));
				m_isHeaderSet = true;

				// If this set of raw data contains header data, note the offset for our message.
				headerOffset = (s_headerSize - startSize) + messageOffset;

				// Message data goes into the next buffer.
				SwapBuffer();
			}
		}
		if (m_isHeaderSet && m_totalBytesParsed < data.size())
		{
			// We want to copy all of the data if we have it, or just the remaining amount of data
			// that we do have.
			int sizeToCopy = std::min(data.size(), m_header->messageLength - m_activeBuffer->size());

			// Note the amount of data copied for this message in case there is header data after
			// these bytes. This way, the header will know where it should begin.
			messageOffset = sizeToCopy + headerOffset;

			// Copy up to full message data into current buffer.
			SetDataOnActiveBuffer(&data[0] +headerOffset, sizeToCopy);
			m_totalBytesParsed += sizeToCopy;

			// If we have a full message.
			if (m_activeBuffer->size() == m_header->messageLength)
			{
				// Package data into NetworkMessage and copy to out-data.
				NetworkMessage msg(*m_header.get());
				msg.messageData = std::move(*m_activeBuffer);
				messages.push_back(std::move(msg));

				// Reset state.
				m_isHeaderSet = false;
				m_header->Clear();
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
		m_firstBuffer.clear();
	}
	else
	{
		m_activeBuffer = &m_firstBuffer;
		m_secondBuffer.clear();
	}
}

void MessageParser::SetDataOnActiveBuffer(const char* data, int size)
{
	std::string& str = *m_activeBuffer;
	int oldSize = str.size();
	str.resize(oldSize + size);
	std::memcpy(&str[0] + oldSize, &data[0], size);
}

//===============================================================================

} // namespace Common
