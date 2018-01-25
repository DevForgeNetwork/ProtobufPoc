//---------------------------------------------------------------
//
// MainScreen.cpp
//

#include "MainScreen.h"
#include "common/Log.h"
#include "common/NetworkController.h"
#include "common/NetworkTypes.h"
#include "common/MessageParser.h"
#include "common/MessageParserTest.h"
#include "common/ProtobufTestDummy.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

#include <windows.h>
#include <assert.h>

namespace Client {

//===============================================================================

	namespace
	{
		const std::string s_fontPath = "resources/fonts/arial.ttf";
	}

	MainScreen::MainScreen(sf::RenderWindow* window)
		: m_window(window)
		, m_font(new sf::Font)
		, m_networkController(new Common::NetworkController)
		, m_protobufTestDummy(new Common::Test::ProtobufTestDummy{ 16, 14, 8, "Dragon" })
		, m_parseTester(new Common::Test::MessageParserTest)
	{
		if (!m_font->loadFromFile(s_fontPath))
		{
			LOG_DEBUG("Error: Could not find font. path=" + s_fontPath);
		}
	}

	MainScreen::~MainScreen()
	{

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

		m_window->draw(m_networkController->IsConnected() ? sendMessageText : connectText);

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
			m_parseTester->RunTests();

		default:
			break;
		}
	}

	void MainScreen::ConnectToServer()
	{
		m_networkController->ConnectToServer(std::string());
	}

	void MainScreen::SendTestMessageToServer()
	{
		auto& creatureBytes = m_protobufTestDummy->ToBytes();

		m_networkController->QueueMessage(Common::MessageType::Creature, creatureBytes.first, creatureBytes.second);
	}

	//===============================================================================

} // namespace Client
