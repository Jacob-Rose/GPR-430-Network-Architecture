#ifndef PLAYER_H
#define PLAYER_H

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

	

	virtual void Update() override;

	//virtual void Serialize(RakNet::BitStream* writeStream) override;
	//virtual void Deserialize(RakNet::BitStream* readStream) override;

protected:

};

#endif