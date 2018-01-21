//---------------------------------------------------------------
//
// main.cpp
//

#include "Common/Log.h"
#include "Common/proto/poc.pb.h"

#include <memory>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

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

namespace {
	const uint32_t s_port = 50001;
	const uint32_t s_minConnectedClients = 1;
}
//class Server
//{
//public:
//	Server()
//	{
//		//LOG_DEBUG("Attempting to listen to port " + s_port);
//		if (m_listener.listen(s_port) != sf::Socket::Done)
//			//LOG_DEBUG("Failed to listen");
//			return;
//	}
//
//	void Run()
//	{
//		while (m_connections.size() < s_minConnectedClients)
//		{
//			
//		}
//	}
//
//private:
//	void CheckConnectionRequests()
//	{
//		std::unique_ptr<Connection> connection = std::make_unique<Connection>();
//		sf::Socket::Status status = m_listener.accept(connection->GetSocket());
//
//
//	}
//
//private:
//	sf::TcpListener m_listener;
//	std::vector<std::unique_ptr<Connection>> m_connections;
//};

//-----------------------------------------------------------------------------------

int main()
{
	Connection c;
	sf::Window window(sf::VideoMode(800, 600), "My window");
	sf::SoundBuffer buffer;
	sf::Font font;
	sf::Text text("Hello SFML", font, 50);

	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}

	// Just makes sure SFML System is linking properly
	sf::Time t1 = sf::seconds(0.1f);
	sf::Int32 milli = t1.asMilliseconds(); // 100
	return 0;
}
