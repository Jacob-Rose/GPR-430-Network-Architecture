#ifndef PLAYER_H
#define PLAYER_H

//SFML
#include <SFML/Graphics/Image.hpp>

//In-House
#include "entity.h"


namespace jr //jakerose namespace
{
	class Player;
}

class jr::Player : public jr::Entity
{
public:
	Player();
	~Player();

	sf::Image m_Sprite;

	virtual void Update() override;

	//virtual void Serialize(RakNet::BitStream* writeStream) override;
	//virtual void Deserialize(RakNet::BitStream* readStream) override;

protected:

};

#endif