#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "entity.h"

#include "shared-net.h"

namespace jr
{
	class Projectile;
}

class jr::Projectile : public jr::Entity
{
public:
	Projectile();
	~Projectile();

	const float PROJECTILE_SPEED = 250.0f;
	const float PROJECTILE_LIFESPAN = 3.0f;
	const float PROJECTILE_DAMAGE = 25.0f;
	const float PROJECTILE_IGNORE_COLLISION_TIME = 0.2f; //used to not hit the player or anything right as it spawns

	float m_TimeAlive = 0.0f;

	void update(EntityUpdateInfo updateInfo) override;


};

#endif