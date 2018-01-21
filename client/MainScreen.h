//---------------------------------------------------------------
//
// MainScreen.h
//

#pragma once

#include "Screen.h"

#include <SFML/Window/Event.hpp>
#include <memory>

namespace sf {
	class RenderWindow;
	class Font;
}

namespace Client {

//===============================================================================

class MainScreen : public Screen
{
public:
	MainScreen(sf::RenderWindow* window);

	// Screen impl.
	virtual void ProcessEvents() override;
	virtual void Draw() override;

private:
	void HandleKeyPress(const sf::Event::KeyEvent& e) const;

private:
	sf::RenderWindow* m_window;
	std::unique_ptr<sf::Font> m_font;

	// Are we connected to the server?
	bool m_isConnected = false;
};

//===============================================================================

} // namespace Client
