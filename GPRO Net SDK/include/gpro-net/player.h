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

	const float PLAYER_SPEED = 100.0f;
	const float PLAYER_ACCEL = 250.0f;
	const float PLAYER_DECCEL = 300.0f;
	const float SHOT_REFRESH_TIME = 0.2f;
	const float PLAYER_SCALE = 0.75f;
	const float PLAYER_MAX_HEALTH = 100.0f;

	const float NETWORK_PACKET_TIME_INTERVAL = 0.033f; //time between sending updates

	float m_TimeSinceShot = 0.0f;
	float m_TimeSincePacketSent = 0.0f;
	float m_Health = PLAYER_MAX_HEALTH;

	int m_KillCount = 0;

	void resetHealth() { m_Health = PLAYER_MAX_HEALTH; }

	virtual void update(EntityUpdateInfo updateInfo) override;

	//https://en.sfml-dev.org/forums/index.php?topic=23624.0
	static bool segmentIntersectsRectangle(const sf::FloatRect& rect,
		const sf::Vector2f& a_p1, const sf::Vector2f& a_p2);

	//virtual void Serialize(RakNet::BitStream* writeStream) override;
	//virtual void Deserialize(RakNet::BitStream* readStream) override;

protected:

};

#endif