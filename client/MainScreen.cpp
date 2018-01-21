//---------------------------------------------------------------
//
// MainScreen.cpp
//

#include "MainScreen.h"
#include "common/Log.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>

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
			if (e.key.code == sf::Keyboard::S)
			{
				// TODO: Send message to server.
				LOG_DEBUG("Sending message to server.");
			}
		}
	}
}

void MainScreen::Draw()
{
	// We must have a valid window to draw to.
	assert(m_window);
	m_window->clear(sf::Color::Black);

	sf::Text text;
	text.setOutlineColor(sf::Color::Red);
	text.setFont(*m_font.get());
	text.setString("Press 's' to send a message to Server.");

	m_window->draw(text);

	m_window->display();
}

//===============================================================================

} // namespace Client
