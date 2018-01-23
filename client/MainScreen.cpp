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

	// Max data per read 64kb.
	const uint32_t s_maxDataChunk = 64000;

	const uint32_t s_headerSize = 8;
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
	void Clear()
	{
		messageType = MessageType::Creature;
		messageLength = 0;
	}
	MessageType messageType = MessageType::Creature;
	int messageLength = 0;
};

using ByteBufferPtr = std::unique_ptr<uint8_t[]>;
class Buffer
{
public:
	uint8_t* GetData() { return m_data.get(); }
	uint32_t GetSize() { return m_size; }

	void InitializeData()
	{
		for (int i = 0; i < s_maxDataChunk; ++i)
		{
			m_data[i] = 0;
		}
	}

	void SetData(const uint8_t data[], uint32_t size)
	{
		if (m_size + size < s_maxDataChunk)
		{
			std::memcpy(m_data.get() + m_size, data, size);
			m_size += size;
			if (m_size > m_maxSinceClear)
			{
				m_maxSinceClear = m_size;
			}
		}
	}

	void ClearData()
	{
		for (uint32_t i = 0; i < m_maxSinceClear; ++i)
		{
			m_data[i] = 0;
		}
		m_size = 0;
		m_maxSinceClear = 0;
	}
private:
	ByteBufferPtr m_data = std::make_unique<uint8_t[]>(s_maxDataChunk);
	uint32_t m_size = 0;

	// Holds the biggest we've been since we've last been cleared. This is just an optimization.
	// This way we don't need to clear the entire buffer, just up to our highest index.
	uint32_t m_maxSinceClear;
};
struct Message
{
	std::unique_ptr<Buffer> messageData = std::make_unique<Buffer>();
	MessageHeader header;
};
class ClientNetworkController
{
	class MessageHelper
	{
	public:

		MessageHelper()
		{
			m_activeBuffer = &m_firstBuffer;
		}

		// TODO: perhaps pass a reference to a Message in. That way I can
		// Clear stale data after we successfully parse a message, instead of
		// at the beginning of this function.
		void ParseMessage(const uint8_t rawData[], uint32_t size, Message& msg)
		{
			// Call site algorithm
			// Receive data
				// datap1 = data
				// While datap1.size() > 0
					// ParseData
					// Handle Message
					// reset datap1
					// memcpy(datap1, data + message.size, size - message.size)

			BuildHeader(rawData, size, msg);

			// Attempt to set the message from our data.
			BuildMessage(rawData, size, msg);
		}

	private:
		void BuildHeader(const uint8_t rawData[], uint32_t& size, Message& msg)
		{
			if (m_header.messageLength == 0)
			{
				int overflow = m_bytesReceivedThisMessage + size - s_headerSize;
				m_bytesReceivedThisMessage += size;
				if (overflow >= 0)
				{
					// Accumulate header data.
					int headerRemainder = size - overflow;
					m_activeBuffer->SetData(rawData, headerRemainder);

					memcpy(&m_header, m_activeBuffer->GetData(), s_headerSize);
					// Put overflow data in next buffer.
					SwapBuffer();
					m_activeBuffer->SetData(rawData + headerRemainder, overflow);

					// We have already handled the header in our old buffer. We shouldn't consider it
					// while parsing the message.
					size -= s_headerSize;
				}
				else
				{
					m_activeBuffer->SetData(rawData, size);
				}
			}
		}

		void BuildMessage(const uint8_t rawData[], uint32_t size, Message& msg)
		{
			if (m_header.messageLength != 0)
			{
				// Calculate number of overflow bytes
				// If we had 3 bytes already, needed 5, but received 12, 10 bytes would be overflowing.
				int overflow = m_activeBuffer->GetSize() - size;

				if (overflow >=0)
				{
					// Copy remainder of the message to our active buffer.
					int messageRemainder = size - overflow;
					m_activeBuffer->SetData(rawData, messageRemainder);

					// Copy full message into our message struct.
					msg.messageData->SetData(m_activeBuffer->GetData(), m_header.messageLength);
					msg.header = m_header;

					// Put the overflow into next buffer.
					SwapBuffer();
					m_activeBuffer->SetData(rawData + messageRemainder, overflow);

					m_header.Clear();
					m_bytesReceivedThisMessage = 0;
				}
				else
				{
					m_activeBuffer->SetData(rawData, size);
				}
			}
		}

		void SwapBuffer()
		{
			if (m_activeBuffer == &m_firstBuffer)
			{
				m_activeBuffer = &m_secondBuffer;
				m_firstBuffer.ClearData();
			}
			else
			{
				m_activeBuffer = &m_firstBuffer;
				m_secondBuffer.ClearData();
			}
		}

		// Whichever buffer is currently in use will be cleared.
		void ClearInactiveBuffer()
		{
			if (m_activeBuffer == &m_firstBuffer)
			{
				m_secondBuffer.ClearData();
			}
			else
			{
				m_firstBuffer.ClearData();
			}
		}

		void ClearActiveBuffer()
		{
			if (m_activeBuffer == &m_firstBuffer)
			{
				m_firstBuffer.ClearData();
			}
			else
			{
				m_firstBuffer.ClearData();
			}
		}

		MessageHeader m_header;
		// Data received will alternate between each buffer.
		Buffer m_firstBuffer;
		Buffer m_secondBuffer;
		Buffer* m_activeBuffer;

		// Total number of bytes received on this message across parse attempts.
		uint32_t m_bytesReceivedThisMessage = 0;
	};

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

	// TODO: Move this.
	MessageHelper m_helper;
private:
	sf::TcpSocket m_socket;
	std::deque<ByteBufferPtr> m_messageQueue;
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

void MainScreen::RunParserTest()
{
	//----------------------------------------------------
	// Data Setup
	//----------------------------------------------------
	MessageHeader header;
	header.messageLength = 14;
	char messageData[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n' };
	auto ClearBuffer = [](uint8_t buffer[], int& size)
	{
		for (int i = 0; i < size; ++i)
		{
			buffer[i] = 0;
		}
		size = 0;
	};

	int size = 50;
	std::unique_ptr<uint8_t[]> netBuffer = std::make_unique<uint8_t[]>(50);
	ClearBuffer(netBuffer.get(), size);

	//----------------------------------------------------

	// Test 1)
	// Receive single message in one chunk
	Message msg;
	assert(sizeof(header) == 8);
	std::memcpy(netBuffer.get(), &header, 8);
	std::memcpy(netBuffer.get() + 8, messageData, 14);
	m_networkController->m_helper.ParseMessage(netBuffer.get(), sizeof(header) + header.messageLength, msg);
	
	assert(msg.header.messageLength == 14);
	assert(msg.header.messageType == MessageType::Creature);
	assert(msg.messageData->GetSize() == 14);
	for (int i = 0; i < 14; ++i)
	{
		assert(msg.messageData->GetData()[i] == messageData[i]);
	}

	// Test 2)
	// Receive the header
	// Receive remainder of message in 3 chunks

	// Test 3)
	// Receive the header
	// Receive remainder of message in 3 chunks
	// Last chunk has part of next message
	// Receive rest of message in next chunk


	// Test 4)
	// Receive the header
	// Receive remainder of message in 3 chunks
	// Last chunk has last chunk + entire next message
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
		break;
	case sf::Keyboard::T:
		RunParserTest();

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
