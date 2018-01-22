//---------------------------------------------------------------
//
// MainScreen.cpp
//

#include "MainScreen.h"
#include "common/Log.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

// Networking stuff.
// TODO: Put somewhere else.
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include "common/proto/poc.pb.h"

#include <vector>
#include <deque>
#include <windows.h>
#include <assert.h>

namespace Client {

//===============================================================================

namespace
{
	const std::string s_fontPath = "resources/fonts/arial.ttf";

	// Network stuff.
	const int s_port = 50001;
	const sf::IpAddress s_serverAddress = "127.0.0.1";
}

// Game data
// TODO: Put this somewhere else. Probably in Common
//--------------------------------------------------------------------------------

enum struct MessageType : uint32_t
{
	Creature
};

struct Creature
{
public:
	int32_t mana;
	int32_t health;
	int32_t speed;
	std::string name;

	Creature FromProtobuf(const poc::Creature& creatureProto)
	{
		return Creature{creatureProto.mana(), creatureProto.health(),
			creatureProto.speed(), creatureProto.name()};
	}

	poc::Creature ToProtobuf()
	{
		poc::Creature creatureProto;
		creatureProto.set_mana(mana);
		creatureProto.set_health(health);
		creatureProto.set_speed(speed);
		creatureProto.set_name(name);

		return creatureProto;
	}
};

//--------------------------------------------------------------------------------

// Networking stuff
// TODO: Put this somewhere else. Probably in Common.
//--------------------------------------------------------------------------------

struct MessageHeader
{
	MessageType messageType;
	uint32_t headerLength;
};

using ByteBuffer = std::unique_ptr<uint8_t[]>;
class ClientNetworkController
{

public:
	ClientNetworkController()
	{
	}

	void SendMessages()
	{
		bool empty = m_messageQueue.empty();
		bool notEmpty = !empty;
		if (!m_messageQueue.empty())
		{
			const auto& message = m_messageQueue.front();

			uint32_t sizeSent = 0;
			sf::TcpSocket::Status status = m_socket.send(message.get(), sizeof(message), sizeSent);

			if (status == sf::TcpSocket::Done)
			{
				m_messageQueue.pop_front();
			}
			else if (status == sf::TcpSocket::Partial)
			{
				// TODO: Handle this case.
				LOG_DEBUG("Error: Only partial message sent to server.");
			}
		}
	}

	void ReceiveMessages()
	{

	}

	void QueueCreatureMessage(Creature* creature)
	{
		// Convert our data to a protobuf message.
		poc::Creature creatureProto = creature->ToProtobuf();

		// Serialize the protobuf message to string.
		std::string serializedProto = creatureProto.SerializeAsString();

		// Add the data to our message header.
		MessageHeader messageHeader{ MessageType::Creature, creatureProto.ByteSize() };

		int headerSize = sizeof(messageHeader);

		// Get ready to push data into a buffer.
		auto buffer = std::make_unique<uint8_t[]>(headerSize + serializedProto.length());

		// Header first.
		std::memcpy(buffer.get(), &messageHeader, headerSize);
		// Then the serialized data.
		std::memcpy(buffer.get() + headerSize, serializedProto.data(), serializedProto.length());

		m_messageQueue.push_front(std::move(buffer));
	}

	bool ConnectToServer()
	{
		while (m_socket.connect(s_serverAddress, s_port) != sf::Socket::Done)
		{
			int ec = WSAGetLastError();
			if (ec)
			{
				if (ec == WSAENETUNREACH)
				{
					LOG_DEBUG("Error: Network unreachable error=" + std::to_string(ec));
					return false;
				}
				else if (ec == WSAETIMEDOUT)
				{
					LOG_DEBUG("Error: Network connection attempt timed out. error=" + std::to_string(ec));
					return false;
				}
				else if (ec == WSAECONNREFUSED)
				{
					LOG_DEBUG("Error: Network connection attempt refused. error=" + std::to_string(ec));
					return false;
				}
				else
				{
					LOG_DEBUG("Error: There was an error attempting to connect to server. error="
						+ std::to_string(ec));
					return false;
				}
			}
		}

		return true;
	}

private:
	sf::TcpSocket m_socket;
	std::deque<ByteBuffer> m_messageQueue;
};

//--------------------------------------------------------------------------------

MainScreen::MainScreen(sf::RenderWindow* window)
	: m_window(window)
	, m_font(new sf::Font)
	, m_networkController(new ClientNetworkController)
	, m_creature(new Creature{16, 14, 8, "Dragon"})
{
	if (!m_font->loadFromFile(s_fontPath))
	{
		LOG_DEBUG("Error: Could not find font. path=" + s_fontPath);
	}
}

void MainScreen::ProcessEvents()
{
	sf::Event e;
	while (m_window->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			m_window->close();
		}

		if (e.type == sf::Event::KeyPressed)
		{
			HandleKeyPress(e.key);
		}
	}

	m_networkController->SendMessages();
}

void MainScreen::Draw()
{
	// We must have a valid window to draw to.
	assert(m_window);
	m_window->clear(sf::Color::Black);

	sf::Text connectText;
	connectText.setOutlineColor(sf::Color::Red);
	connectText.setFont(*m_font.get());
	connectText.setString("Press 'c' to connect to Server.");

	sf::Text sendMessageText;
	sendMessageText.setOutlineColor(sf::Color::Red);
	sendMessageText.setFont(*m_font.get());
	sendMessageText.setString("Press 's' to send a message to Server.");

	m_window->draw(m_isConnected ? sendMessageText : connectText);

	m_window->display();
}

void MainScreen::HandleKeyPress(const sf::Event::KeyEvent& e)
{
	switch (e.code)
	{
	case sf::Keyboard::C:
		ConnectToServer();
		break;
	case sf::Keyboard::S:
		SendTestMessageToServer();
		LOG_DEBUG("Sending message to server.");
		break;
	default:
		break;
	}
}

void MainScreen::ConnectToServer()
{
	if (!m_isConnected)
	{
		LOG_DEBUG("Connecting to Server.");
		m_isConnected = m_networkController->ConnectToServer();
	}
}

void MainScreen::SendTestMessageToServer()
{
	m_networkController->QueueCreatureMessage(m_creature.get());
}

//===============================================================================

} // namespace Client
