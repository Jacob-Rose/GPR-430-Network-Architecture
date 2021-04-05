#ifndef ENEMY_H
#define ENEMY_H

//SFML
#include <SFML/Graphics.hpp>

//In-House
#include "entity.h"

namespace jr
{
	class Enemy;
}

class jr::Enemy : public jr::Entity
{
public:
	Enemy();
	~Enemy();


	virtual void Update() override;
protected:

};

#endif