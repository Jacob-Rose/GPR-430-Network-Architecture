#ifndef _SHARED_NET_H
#define _SHARED_NET_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

struct GameUpdateInfo
{
public:
	sf::Time dt;
};

struct EntityUpdateInfo : public GameUpdateInfo
{
public:
	bool isOwner;
	sf::Window* window;
};

#endif