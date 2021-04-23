#include "gpro-net/player.h"

#include "gpro-net/resource-manager.h"

#include <RakNet/RakPeerInterface.h>
#include <gpro-net/network-messages.h>

jr::Player::Player()
{
	m_Sprite.setTexture(*jr::ResourceManager::getSingleton()->getTextureFromID(jr::ResourceManager::PLAYER_TEXTURE_ID)); // a mouthful, but it really is clean as hell
	m_ArmSprite.setTexture(*jr::ResourceManager::getSingleton()->getTextureFromID(jr::ResourceManager::PLAYER_ARM_TEXTURE_ID)); // a mouthful, but it really is clean as hell
	m_ArmSprite.setOrigin(m_Sprite.getLocalBounds().width * 0.5f, 0.0f);
	m_ArmSprite.setScale(0.25f, 0.25f);
	m_Sprite.setOrigin(m_Sprite.getLocalBounds().width * 0.5f, m_Sprite.getLocalBounds().height * 0.5f);
	m_Sprite.setScale(0.1f, 0.1f);
}

jr::Player::~Player()
{
}

void jr::Player::update(EntityUpdateInfo updateInfo)
{
	m_TimeSinceShot += updateInfo.dt.asSeconds();
	float relativeRot = m_Rotation;
	if (updateInfo.isOwner && updateInfo.window->hasFocus())
	{
		m_Velocity.x = 0;
		m_Velocity.y = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		{
			m_Velocity.y -= PLAYER_SPEED;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			m_Velocity.y += PLAYER_SPEED;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		{
			m_Velocity.x -= PLAYER_SPEED;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		{
			m_Velocity.x += PLAYER_SPEED;
		}

		//Handle Rotation
		sf::Vector2i pos = sf::Mouse::getPosition(*updateInfo.window);

		sf::Vector2f mousePos_world = sf::Vector2f(updateInfo.window->mapPixelToCoords(pos));

		sf::Vector2f dir = m_Position - mousePos_world;

		//https://stackoverflow.com/questions/40776703/converting-from-radians-to-degrees
		m_Rotation = std::atan2(dir.y, dir.x) * (180.0f / 3.141592653589793238463f); //to degrees

		if (m_TimeSinceShot > SHOT_REFRESH_TIME)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				
				NetworkObjectRequestCreateMessage* msg = new NetworkObjectRequestCreateMessage();
				msg->objectType = NetworkObjectID::BULLET_OBJECT_ID;
				msg->objectPos[0] = m_Position.x;
				msg->objectPos[1] = m_Position.y;
				msg->objectRot = relativeRot;
				m_RemoteOutputCache.push_back(msg);
				
			}
			m_TimeSinceShot = 0.0f;
		}

		

		NetworkObjectUpdateMessage* msg = new NetworkObjectUpdateMessage();
		msg->m_NetID = m_NetID;
		msg->newPos[0] = m_Position.x;
		msg->newPos[1] = m_Position.y;
		msg->newRot = relativeRot;

		m_RemoteOutputCache.push_back(msg);
	}

	//All players and clients do this stuff

	m_Position += m_Velocity * updateInfo.dt.asSeconds();


	//Now Everyone does this
	m_Sprite.setPosition(m_Position);

	m_Sprite.setRotation(0);

	m_ArmSprite.setRotation(relativeRot + 10.0f);

	sf::Vector2f armOffset;
	const float DEGTORAD = 0.01745329252f;
	armOffset.x = std::cos(relativeRot * DEGTORAD) * 1.0f;
	armOffset.y = std::sin(relativeRot * DEGTORAD) * 1.0f;
	m_ArmSprite.setPosition(m_Position + armOffset + sf::Vector2f(-0.1f, 0.0f));
}
