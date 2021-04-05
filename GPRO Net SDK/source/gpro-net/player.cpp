#include "gpro-net/player.h"

jr::Player::Player()
{
	m_Texture.loadFromFile("resources/textures/player.png");
	m_Sprite.setTexture(m_Texture);
}

jr::Player::~Player()
{
}

void jr::Player::Update()
{

}
