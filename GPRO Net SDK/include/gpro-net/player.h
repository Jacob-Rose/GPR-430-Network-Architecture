#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>

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

	const float PLAYER_SPEED = 25.0f;
	const float SHOT_REFRESH_TIME = 1.0f;

	float m_TimeSinceShot = 0.0f;

	sf::Sprite m_ArmSprite;
	

	virtual void update(EntityUpdateInfo updateInfo) override;

	//virtual void Serialize(RakNet::BitStream* writeStream) override;
	//virtual void Deserialize(RakNet::BitStream* readStream) override;

protected:

};

#endif