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
	enum struct MessageType : uint32_t;
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
	void SendMessageToServer(Common::MessageType type, uint8_t message[], uint32_t messageLength);

	bool IsConnected() const;

private:
	std::unique_ptr<Common::NetworkHelper> m_networkHelper;
	std::unique_ptr<Common::Connection> m_connection;

	bool m_shouldConnect = false;
};

//===============================================================================

} // namespace Client
