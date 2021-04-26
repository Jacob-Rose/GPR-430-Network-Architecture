#ifndef TILE_ENTITY_H
#define TILE_ENTITY_H

#include "entity.h"
#include "resource-manager.h"

namespace jr
{
	class TileEntity;
}

class jr::TileEntity : public jr::Entity
{
public:
	const float m_Scale  = 1.0f;

	sf::Vector2i m_Position;
	char m_TileID;


	TileEntity(int tileID, sf::Vector2i pos) : Entity(), m_TileID(tileID), m_Position(pos)
	{
		switch (tileID)
		{
		case 0:
			m_Sprite.setTexture(*jr::ResourceManager::getSingleton()->getTextureFromID(jr::ResourceManager::TILE_GRASS_ID));
			break;
		case 1:
			m_Sprite.setTexture(*jr::ResourceManager::getSingleton()->getTextureFromID(jr::ResourceManager::TILE_WALL_ID));

		}

		m_Sprite.setPosition(sf::Vector2f(pos.x * m_Sprite.getGlobalBounds().width * m_Scale, pos.y * m_Sprite.getGlobalBounds().height * m_Scale));
		m_Sprite.setScale(sf::Vector2f(m_Scale, m_Scale));
	}
	~TileEntity()
	{

	}

	void setTileID()
	{

	}

	void update(EntityUpdateInfo updateInfo) override {}

};

#endif