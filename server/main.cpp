//---------------------------------------------------------------
//
// main.cpp
//

#include "Common/Log.h"

#include <memory>
#include <SFML/Network.hpp>

namespace {
	const uint32_t s_port = 50001;
	const uint32_t s_minConnectedClients = 1;
}

class Connection
{
public:
	Connection()
		: m_isConnected(false)
	{
		m_socketConnection.setBlocking(false);
	}

	sf::TcpSocket& GetSocket() { return m_socketConnection; }
	bool IsConnnected() { return m_isConnected; }

	// These two are separate functions for verbosity.
	void Disconnect() { m_isConnected = false; }
	void Connect() { m_isConnected = true; }

private:
	sf::TcpSocket m_socketConnection;
	bool m_isConnected;
};

//-----------------------------------------------------------------------------------

class Server
{
public:
	Server()
	{
		LOG_DEBUG_CONSOLE("Attempting to listen to port ");

		// Returns Done if opening the port was successful.
		if (m_listener.listen(s_port) != sf::Socket::Done)
		{
			LOG_DEBUG_CONSOLE("Failed to listen.");
		}
	}

	void Run()
	{
		while (m_connections.size() < s_minConnectedClients)
		{
			CheckConnectionRequests();
		}
	}

private:
	void CheckConnectionRequests()
	{
		std::unique_ptr<Connection> connection = std::make_unique<Connection>();
		sf::Socket::Status status = m_listener.accept(connection->GetSocket());
		LOG_DEBUG_CONSOLE("status" + std::to_string(status));
	}

private:
	sf::TcpListener m_listener;
	std::vector<std::unique_ptr<Connection>> m_connections;
};

//-----------------------------------------------------------------------------------

int main()
{
	Server server;
	server.Run();
	return 0;
}