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

	const float PLAYER_SPEED = 5.0f;

	

	virtual void Update(EntityUpdateInfo updateInfo) override;

	//virtual void Serialize(RakNet::BitStream* writeStream) override;
	//virtual void Deserialize(RakNet::BitStream* readStream) override;

protected:

};

#endif