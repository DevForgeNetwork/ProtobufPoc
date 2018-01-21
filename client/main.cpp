//---------------------------------------------------------------
//
// main.cpp
//

#include <SFML/System.hpp>

int main()
{
	sf::Time t1 = sf::seconds(0.1f);
	sf::Int32 milli = t1.asMilliseconds(); // 100
	return 0;
}
