#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/System/Vector2.hpp>

#include <RakNet/RakNetTypes.h>

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

	virtual void Update();

	friend class GameState;
protected:


	//Should be called by the client or server
	Entity();
	~Entity();

};

#endif