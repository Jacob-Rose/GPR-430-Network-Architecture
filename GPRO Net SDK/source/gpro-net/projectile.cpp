#include "gpro-net/projectile.h"
#include "gpro-net/resource-manager.h"
#include "gpro-net/game-state.h"
#include <math.h>

jr::Projectile::Projectile()
{
	m_Sprite.setTexture(*jr::ResourceManager::getSingleton()->getTextureFromID(jr::ResourceManager::PROJECTILE_TEXTURE_ID)); // a mouthful, but it really is clean as hell
	m_Sprite.setOrigin(m_Sprite.getLocalBounds().width * 0.5f, m_Sprite.getLocalBounds().height * 0.5f);
	m_Sprite.setScale(0.25f, 0.25f);
}

jr::Projectile::~Projectile()
{
}

void jr::Projectile::update(EntityUpdateInfo updateInfo)
{
	const float DEGTORAD = 0.01745329252f;
	m_TimeAlive += updateInfo.dt.asSeconds();
	//https://www.rapidtables.com/convert/number/how-degrees-to-radians.html
	m_Velocity.x = std::cos(m_Rotation * DEGTORAD) * -PROJECTILE_SPEED;
	m_Velocity.y = std::sin(m_Rotation * DEGTORAD) * -PROJECTILE_SPEED;


	m_Position += m_Velocity * updateInfo.dt.asSeconds();

	m_Sprite.setPosition(m_Position);
	m_Sprite.setRotation(m_Rotation + 90.0f);
	if (updateInfo.isOwner && m_TimeAlive > PROJECTILE_LIFESPAN)
	{
		m_DeleteMe = true;
	}
}