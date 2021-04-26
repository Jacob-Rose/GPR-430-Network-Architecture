#ifndef _SHARED_NET_H
#define _SHARED_NET_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

namespace jr
{
	class GameState;

	struct GameUpdateInfo
	{
	public:
		sf::Time dt;
		jr::GameState* gameState;
	};

	struct EntityUpdateInfo : public GameUpdateInfo
	{
	public:
		bool isOwner;
	};

}

#endif