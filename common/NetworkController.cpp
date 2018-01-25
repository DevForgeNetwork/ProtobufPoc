//---------------------------------------------------------------
//
// NetworkController.cpp
//

#include "NetworkController.h"
#include "NetworkTypes.h"
#include "MessageParser.h"
#include "Log.h"

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <windows.h>

namespace Common {

//===============================================================================

namespace {
	// Network stuff.
	const int s_port = 50001;
	const sf::IpAddress s_serverAddress = "127.0.0.1";
}

NetworkController::NetworkController()
	: m_parser(new MessageParser)
	, m_socket(new sf::TcpSocket)
{
}

NetworkController::~NetworkController()
{
}

void NetworkController::SendMessages()
{
	bool empty = m_messageQueue.empty();
	bool notEmpty = !empty;
	if (!m_messageQueue.empty())
	{
		const auto& message = m_messageQueue.front();

		uint32_t sizeSent = 0;
		sf::TcpSocket::Status status = m_socket->send(message.get(), sizeof(message), sizeSent);
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
			m_isConnected = false;
		}
	}
}

void NetworkController::ReceiveMessages()
{

}

void NetworkController::QueueMessage(MessageType type, uint8_t message[], uint32_t messageLength)
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

void NetworkController::ConnectToServer(const std::string& address)
{
	if (m_isConnected)
	{
		return;
	}

	sf::IpAddress ipAddress = address.empty() ? s_serverAddress : address;

	while (m_socket->connect(ipAddress, s_port) != sf::Socket::Done)
	{
		int ec = WSAGetLastError();
		if (ec)
		{
			if (ec == WSAENETUNREACH)
			{
				LOG_DEBUG("Error: Network unreachable error=" + std::to_string(ec));
			}
			else if (ec == WSAETIMEDOUT)
			{
				LOG_DEBUG("Error: Network connection attempt timed out. error=" + std::to_string(ec));
			}
			else if (ec == WSAECONNREFUSED)
			{
				LOG_DEBUG("Error: Network connection attempt refused. error=" + std::to_string(ec));
			}
			else
			{
				LOG_DEBUG("Error: There was an error attempting to connect to server. error="
					+ std::to_string(ec));
			}
		}
	}

	m_isConnected = true;
}

//===============================================================================

} // namespace Common
