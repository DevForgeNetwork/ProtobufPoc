//---------------------------------------------------------------
//
// main.cpp
//

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

int main()
{
	sf::Window window(sf::VideoMode(800, 600), "My window");
	sf::Time t1 = sf::seconds(0.1f);
	sf::Int32 milli = t1.asMilliseconds();
	return 0;
}
