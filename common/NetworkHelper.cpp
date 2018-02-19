//---------------------------------------------------------------
//
// NetworkHelper.cpp
//

#include "NetworkHelper.h"
#include "NetworkTypes.h"
#include "MessageParser.h"
#include "Log.h"

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <windows.h>

namespace Common {

//===============================================================================

Connection::Connection()
{
	socket->setBlocking(false);
}

Connection::Connection(Connection&& c)
	: socket(std::move(c.socket))
{

}

Connection::~Connection()
{
}

bool Connection::IsConnected()
{
	return socket->getRemoteAddress() != sf::IpAddress::None;
}

//-------------------------------------------------------------------------------

NetworkHelper::NetworkHelper()
	: m_parser(new MessageParser)
{
}

NetworkHelper::~NetworkHelper()
{
}

void NetworkHelper::SendMessages(Connection& connection)
{
	bool empty = m_messageQueue.empty();
	bool notEmpty = !empty;
	if (!m_messageQueue.empty())
	{
		const auto& message = m_messageQueue.front();

		uint32_t sizeSent = 0;
		sf::TcpSocket::Status status = connection.socket->send(message.get(), sizeof(message), sizeSent);
		if (status == sf::TcpSocket::Done)
		{
			m_messageQueue.pop_front();
		}
		else if (status == sf::TcpSocket::Partial)
		{
			// TODO: Handle this case.
			LOG_DEBUG("Error: Only partial message sent to server.");
		}
		else if (status == sf::TcpSocket::Disconnected)
		{
			int ec = WSAGetLastError();

			LOG_DEBUG("Error: Disconnected.error=" + std::to_string(ec));
			m_messageQueue.clear();
			connection.socket->disconnect();
		}
	}
}

void NetworkHelper::ReceiveMessages(Connection& connection)
{
	// NYI: populate the messages with whatever we received.
}

void NetworkHelper::QueueMessage(MessageType type, uint8_t message[], uint32_t messageLength)
{
	LOG_DEBUG("Queuing Message to send to Server");
	// Add the data to our message header.
	MessageHeader messageHeader{ type, messageLength };

	int headerSize = sizeof(messageHeader);

	// Get ready to push data into a buffer.
	auto buffer = std::make_unique<uint8_t[]>(headerSize + messageLength);

	// Header first.
	std::memcpy(buffer.get(), &messageHeader, headerSize);
	// Then the serialized data.
	std::memcpy(buffer.get() + headerSize, message, messageLength);

	m_messageQueue.push_front(std::move(buffer));
}

//===============================================================================

} // namespace Common
