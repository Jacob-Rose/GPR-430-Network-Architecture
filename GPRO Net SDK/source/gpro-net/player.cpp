#include "gpro-net/player.h"

#include <RakNet/RakPeerInterface.h>

jr::Player::Player()
{
	m_Texture.loadFromFile("resources/textures/player.png");
	m_Sprite.setTexture(m_Texture);
}

jr::Player::~Player()
{
}

void jr::Player::Update(EntityUpdateInfo updateInfo)
{
	if (updateInfo.isOwner)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		{
			m_Position.y += updateInfo.dt.asSeconds() * PLAYER_SPEED;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			m_Position.y -= updateInfo.dt.asSeconds() * PLAYER_SPEED;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		{
			m_Position.x += updateInfo.dt.asSeconds() * PLAYER_SPEED;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		{
			m_Position.x -= updateInfo.dt.asSeconds() * PLAYER_SPEED;
		}
	}

	//Now Everyone does this
	m_Sprite.setPosition(m_Position);
}
