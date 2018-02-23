//---------------------------------------------------------------
//
// MainScreen.cpp
//

#include "MainScreen.h"

#include "client/Application.h"
#include "client/NetworkController.h"
#include "common/Log.h"
#include "common/MessageParser.h"
#include "common/MessageParserTest.h"
#include "common/NetworkTypes.h"
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

MainScreen::MainScreen(Application* application, sf::RenderWindow* window)
	: m_window(window)
	, m_font(new sf::Font)
	, m_application(application)
	, m_protobufTestDummy(new Common::Test::ProtobufTestDummy{ -16, 14, 8, "Dragon" })
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

	if (m_shouldConnect)
	{
		ConnectToServer();
	}
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

	m_window->draw(m_application->GetNetworkController()->IsConnected() ? sendMessageText : connectText);

	m_window->display();
}

void MainScreen::HandleKeyPress(const sf::Event::KeyEvent& e)
{
	switch (e.code)
	{
	case sf::Keyboard::C:
		m_shouldConnect = true;
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
	NetworkController* controller = m_application->GetNetworkController();
	controller->ConnectToServer();

	if (controller->IsConnected())
	{
		m_shouldConnect = false;
	}
}

void MainScreen::SendTestMessageToServer()
{
	auto creatureString = m_protobufTestDummy->ToString();

	m_application->GetNetworkController()->SendMessageToServer(Common::MessageId::Creature,
		creatureString);
}

//===============================================================================

} // namespace Client
