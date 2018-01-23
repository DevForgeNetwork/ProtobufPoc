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

struct Creature;
class ClientNetworkController;

class MainScreen : public Screen
{
public:
	MainScreen(sf::RenderWindow* window);

	// Screen impl.
	virtual void ProcessEvents() override;
	virtual void Draw() override;

private:
	void HandleKeyPress(const sf::Event::KeyEvent& e);
	void ConnectToServer();
	void SendTestMessageToServer();

	void RunParserTest();
private:
	sf::RenderWindow* m_window;
	std::unique_ptr<sf::Font> m_font;

	std::unique_ptr<Creature> m_creature;
	std::unique_ptr<ClientNetworkController> m_networkController;

	// Are we connected to the server?
	bool m_isConnected = false;
};

//===============================================================================

} // namespace Client
