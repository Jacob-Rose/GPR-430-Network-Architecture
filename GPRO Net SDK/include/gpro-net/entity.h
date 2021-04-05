#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <RakNet/RakNetTypes.h>

#include "gpro-net/shared-net.h"

namespace jr
{
	class Entity;
	class GameState;
}

class jr::Entity
{
public:
	unsigned int m_NetID;
	RakNet::RakNetGUID m_OwnerAddress;

	sf::Vector2f m_Position;
	float m_Rotation;

	virtual void Update(EntityUpdateInfo updateInfo);

	friend class GameState;
protected:
	sf::Sprite m_Sprite;
	sf::Texture m_Texture; //could be static so all players use same tex resource, but im lazy and worried about time

	//Should be called by the client or server
	Entity();
	~Entity();

};

#endif