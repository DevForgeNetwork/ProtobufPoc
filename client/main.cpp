//---------------------------------------------------------------
//
// main.cpp
//

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

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

int main()
{
	Connection c;
	sf::Window window(sf::VideoMode(800, 600), "My window");

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
