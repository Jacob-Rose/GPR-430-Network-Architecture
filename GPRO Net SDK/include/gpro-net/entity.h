#ifndef ENTITY_H
#define ENTITY_H

//SFML
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

//RakNet
#include <RakNet/RakNetTypes.h>

//In House
#include <gpro-net/shared-net.h>
#include <gpro-net/network-messages.h>
#include <gpro-net/netid.h>

namespace jr
{
	class Entity;
	class GameState; //just a def to help us friend it
}

class jr::Entity
{
public:
	//Should be called by the client or server
	Entity();
	~Entity();

	jr::NetID m_NetID;
	RakNet::RakNetGUID m_OwnerAddress;

	sf::Vector2f m_Position;
	sf::Vector2f m_NewPosition; //for dead reckoning

	sf::Vector2f m_Velocity; //useful for network prediction
	sf::Vector2f m_NewVelocity; //for dead reckoning
	
	float m_Rotation;
	float m_NewRotation; //for dead reckoning

	sf::Sprite m_Sprite;

	virtual void update(EntityUpdateInfo updateInfo) = 0;
	//std::vector<jr::Entity> m_ChildObjects = std::vector<jr::Entity>();


	std::vector<NetworkMessage*> m_RemoteOutputCache = std::vector<NetworkMessage*>();


	friend class GameState;
protected:
	bool m_DeleteMe = false; //indicates ready to be deleted


};

#endif