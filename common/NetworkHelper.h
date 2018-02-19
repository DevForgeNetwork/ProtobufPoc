//---------------------------------------------------------------
//
// NetworkHelper.h
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
struct NetworkMessage;

class Connection
{
public:
	Connection();
	~Connection();
	Connection(Connection&& c);
	bool IsConnected();

	std::unique_ptr<sf::TcpSocket> socket = std::make_unique<sf::TcpSocket>();
};

class NetworkHelper
{
public:
	NetworkHelper();
	~NetworkHelper();

	// Called in the event loop. Will send all queued messages.
	void SendMessages(Connection& connection);

	// Called in the event loop. Will receive up to a certain amount of bytes per call.
	void ReceiveMessages(Connection& connection);

	// Queues a message to be sent the next time SendMessages is called. The type is needed so it can be
	// packaged with the message.
	void QueueMessage(MessageType type, uint8_t message[], uint32_t messageLength);

private:
	std::unique_ptr<MessageParser> m_parser;
	std::deque<std::unique_ptr<uint8_t[]>> m_messageQueue;
};

//===============================================================================

} // namespace Common
