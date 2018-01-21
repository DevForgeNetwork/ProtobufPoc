//---------------------------------------------------------------
//
// MainScreen.h
//

#pragma once

#include "Screen.h"

namespace sf {
	class RenderWindow;
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
};

//===============================================================================

} // namespace Client
