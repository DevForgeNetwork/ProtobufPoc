//---------------------------------------------------------------
//
// NetworkController.h
//

#pragma once

#include <string>
#include <memory>

namespace Common {
	class Connection;
	class NetworkHelper;
	enum struct MessageId : uint32_t;
}

namespace Client {

//===============================================================================

class NetworkController
{
public:
	NetworkController();
	~NetworkController();

	// Connect to the server at the specified address.
	void ConnectToServer(const std::string& address = std::string(), int port = 0);

	// Will queue a message up to get sent to the server.
	void SendMessageToServer(Common::MessageId type, const std::string& message);

	// Called from the main loop. This will send any queued messages and receive anything from over the wire.
	void Process();

	// Returns whether we're currently connected to the server.
	bool IsConnected() const;

private:
	std::unique_ptr<Common::NetworkHelper> m_networkHelper;
	std::unique_ptr<Common::Connection> m_connection;

	bool m_shouldConnect = false;
};

//===============================================================================

} // namespace Client
