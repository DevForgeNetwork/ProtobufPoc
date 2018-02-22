//---------------------------------------------------------------
//
// Application.cpp
//

#include "Application.h"

#include "client/MainScreen.h"
#include "client/NetworkController.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace Client {

//===============================================================================

Application::Application()
	: m_mainWindow(new sf::RenderWindow(sf::VideoMode(640, 480), "Client"))
	, m_screen(new MainScreen(this, m_mainWindow.get()))
	, m_networkController(new NetworkController)
{
	m_mainWindow->setFramerateLimit(60);
}

Application::~Application()
{
}

void Application::Run()
{
	while (m_mainWindow->isOpen())
	{
		m_screen->ProcessEvents();
		m_screen->Draw();

		m_networkController->Process();
	}
}

//===============================================================================

} // namespace Client
