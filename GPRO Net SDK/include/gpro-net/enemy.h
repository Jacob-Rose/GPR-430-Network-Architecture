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

class jr::Enemy : jr::Entity
{
public:
	Enemy();
	~Enemy();
protected:

};

#endif