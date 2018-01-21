//---------------------------------------------------------------
//
// MainScreen.h
//

#pragma once

#include "Screen.h"

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
	sf::RenderWindow* m_window;
	std::unique_ptr<sf::Font> m_font;
};

//===============================================================================

} // namespace Client
