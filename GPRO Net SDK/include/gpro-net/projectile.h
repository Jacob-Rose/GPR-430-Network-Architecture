#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "entity.h"

namespace jr
{
	class Projectile;
}

class jr::Projectile : public jr::Entity
{
public:
	Projectile();
	~Projectile();

	const float PROJECTILE_SPEED = 90.0f;
	const float PROJECTILE_LIFESPAN = 10.0f;

	float m_TimeAlive = 0.0f;

	void update(EntityUpdateInfo updateInfo) override;

};

#endif