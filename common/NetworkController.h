//---------------------------------------------------------------
//
// NetworkController.h
//

#pragma once

#include <memory>
#include <deque>

namespace sf {
	class IpAddress;
	class TcpSocket;
}

namespace Common {

//===============================================================================

enum struct MessageType : uint32_t;
class MessageParser;
class NetworkController
{
public:
	NetworkController();
	~NetworkController();

	// Called in the event loop. Will send all queued messages.
	void SendMessages();

	// Called in the event loop. Will receive up to a certain amount of bytes per call.
	void ReceiveMessages();

	// Queues a message to be sent the next time SendMessages is called. The type is needed so it can be
	// packaged with the message.
	void QueueMessage(MessageType type, uint8_t message[], uint32_t messageLength);

	// Connect to the server at the specified address.
	void ConnectToServer(const std::string& address);

	// Returns whether we're connected or not.
	bool IsConnected() const { return m_isConnected; }

private:
	std::unique_ptr<MessageParser> m_parser;
	std::unique_ptr<sf::TcpSocket> m_socket;
	std::deque<std::unique_ptr<uint8_t[]>> m_messageQueue;
	bool m_isConnected = false;
};

//===============================================================================

} // namespace Common
