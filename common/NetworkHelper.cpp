//---------------------------------------------------------------
//
// NetworkHelper.cpp
//

#include "NetworkHelper.h"
#include "NetworkTypes.h"
#include "MessageParser.h"
#include "ProtobufTestDummy.h"
#include "Log.h"

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <utility>
#include <windows.h>

namespace Common {

//===============================================================================

namespace {
	const int s_maxSizeReceived = 512;
}

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

void NetworkHelper::SendMessages(Connection* connection)
{
	if (!connection)
	{
		LOG_DEBUG("Error: tried to send messages on a dead connection.");
		return;
	}

	bool empty = m_messageQueue.empty();
	bool notEmpty = !empty;
	if (!m_messageQueue.empty())
	{
		const auto& message = m_messageQueue.front();

		std::vector<char> fullMessage(sizeof(message.first) + message.second.size());
		std::memcpy(fullMessage.data(), &message.first, sizeof(message.first));
		std::memcpy(fullMessage.data() + sizeof(message.first), message.second.c_str(), message.second.size());

		uint32_t sizeSent = 0;
		sf::TcpSocket::Status status = connection->socket->send(fullMessage.data(), fullMessage.size(), sizeSent);
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
			connection->socket->disconnect();
		}
	}
}

void NetworkHelper::ReceiveMessages(Connection* connection)
{
	if (!connection)
	{
		LOG_DEBUG("Error: tried to reveive messages on a dead connection.");
		return;
	}

	// NYI: populate the messages with whatever we received.
	uint32_t received = 0;
	uint8_t data[s_maxSizeReceived];
	connection->socket->receive(data, s_maxSizeReceived, received);

	std::vector<NetworkMessage> messages;
	m_parser->ParseMessage(data, received, messages);

	if (!messages.empty())
	{
		LOG_DEBUG("Receving messages over the wire succeeded.");
		HandleMessages(messages);
	}
}

void NetworkHelper::QueueMessage(MessageType type, const std::string& message)
{
	LOG_DEBUG("Queuing Message to send to Server");

	auto messageLength = message.size();
	// Add the data to our message header.
	MessageHeader messageHeader { type, messageLength };

	m_messageQueue.emplace_front(messageHeader, message);
}

void NetworkHelper::HandleMessages(const std::vector<NetworkMessage>& messages)
{
	for (auto it = std::cbegin(messages); it != std::cend(messages); ++it)
	{
		if (it->header.messageType == MessageType::Creature)
		{
			Common::Test::ProtobufTestDummy dummy(std::make_pair(it->messageData.GetData(),
				it->header.messageLength));

			LOG_DEBUG("ProtobufDummy Creation Succeeded."
				" name=" + dummy.GetName() +
				" health=" + std::to_string(dummy.GetHealth()) +
				" mana=" + std::to_string(dummy.GetMana()) +
				" speed=" + std::to_string(dummy.GetSpeed()))
		}
	}
}

//===============================================================================

} // namespace Common
