#include "gpro-net/player.h"

#include "gpro-net/resource-manager.h"
#include "gpro-net/game-state.h"

#include "gpro-net/tile.hpp"

#include <RakNet/RakPeerInterface.h>
#include <gpro-net/network-messages.h>


jr::Player::Player()
{
	m_Sprite.setTexture(*jr::ResourceManager::getSingleton()->getTextureFromID(jr::ResourceManager::PLAYER_TEXTURE_ID)); // a mouthful, but it really is clean as hell
	m_Sprite.setOrigin(m_Sprite.getLocalBounds().width * 0.5f, m_Sprite.getLocalBounds().height * 0.5f);
	m_Sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
}

jr::Player::~Player()
{
}

void jr::Player::update(EntityUpdateInfo updateInfo)
{
	//Handle Timers
	m_TimeSinceShot += updateInfo.dt.asSeconds();
	m_TimeSincePacketSent += updateInfo.dt.asSeconds();

	//What owners do
	if (updateInfo.isOwner)
	{
		if (updateInfo.gameState->m_GameWindow->hasFocus())
		{
			bool xActive = false, yActive = false;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
			{
				m_Velocity.y -= PLAYER_ACCEL * updateInfo.dt.asSeconds();
				yActive = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			{
				m_Velocity.y += PLAYER_ACCEL * updateInfo.dt.asSeconds();
				yActive = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			{
				m_Velocity.x -= PLAYER_ACCEL * updateInfo.dt.asSeconds();
				xActive = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			{
				m_Velocity.x += PLAYER_ACCEL * updateInfo.dt.asSeconds();
				xActive = true;
			}
			m_Velocity.x = std::max(std::min(m_Velocity.x, PLAYER_SPEED), -PLAYER_SPEED);
			m_Velocity.y = std::max(std::min(m_Velocity.y, PLAYER_SPEED), -PLAYER_SPEED);

			if (!xActive)
			{
				if (m_Velocity.x > 0)
				{
					m_Velocity.x -= PLAYER_DECCEL * updateInfo.dt.asSeconds();
					m_Velocity.x = std::max(m_Velocity.x, 0.0f);
				}
				else
				{
					m_Velocity.x += PLAYER_DECCEL * updateInfo.dt.asSeconds();
					m_Velocity.x = std::min(m_Velocity.x, 0.0f);
				}

			}
			if (!yActive)
			{
				if (m_Velocity.y > 0)
				{
					m_Velocity.y -= PLAYER_DECCEL * updateInfo.dt.asSeconds();
					m_Velocity.y = std::max(m_Velocity.y, 0.0f);
				}
				else
				{
					m_Velocity.y += PLAYER_DECCEL * updateInfo.dt.asSeconds();
					m_Velocity.y = std::min(m_Velocity.y, 0.0f);
				}
			}

			sf::Vector2f proposedNewPos = m_Position + (m_Velocity * updateInfo.dt.asSeconds());
			for (int i = 0; i < updateInfo.gameState->m_EntityLayers[(int)GameState::Layers::WALLS].size(); ++i)
			{
				jr::TileEntity* tile = static_cast<TileEntity*>(updateInfo.gameState->m_EntityLayers[(int)GameState::Layers::WALLS][i]);
				sf::FloatRect rect = m_Sprite.getGlobalBounds();
				rect.left += proposedNewPos.x - m_Position.x;
				rect.top += proposedNewPos.y - m_Position.y;
				if (rect.intersects(tile->m_Sprite.getGlobalBounds()))
				{
					m_Velocity = sf::Vector2f(0.0f, 0.0f);
				}
			}

			//Handle Rotation
			sf::Vector2i pos = sf::Mouse::getPosition(*updateInfo.gameState->m_GameWindow);
			sf::Vector2f mousePos_world = sf::Vector2f(updateInfo.gameState->m_GameWindow->mapPixelToCoords(pos));
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
					msg->objectRot = m_Rotation;
					m_RemoteOutputCache.push_back(msg);

				}
				m_TimeSinceShot = 0.0f;
			}
		}
		else
		{
			m_Velocity = sf::Vector2f(0.0f, 0.0f);
		}
		


		
		
		if (m_TimeSincePacketSent > NETWORK_PACKET_TIME_INTERVAL)
		{
			NetworkObjectUpdateMessage* msg = new NetworkObjectUpdateMessage();
			msg->m_NetID = m_NetID;
			msg->newPos[0] = m_Position.x;
			msg->newPos[1] = m_Position.y;
			msg->newVelocity[1] = m_Velocity.y;
			msg->newVelocity[1] = m_Velocity.y;
			msg->newRot = m_Rotation;

			m_RemoteOutputCache.push_back(msg);
			m_TimeSincePacketSent = 0.0f;
		}

		//We are not getting updates from the server, this is for us
		m_NewPosition = m_Position;
		m_NewVelocity = m_Velocity;
		m_NewRotation = m_Rotation;

	}
	//All players and clients do this stuff
	m_Position = m_Position + (m_Velocity * updateInfo.dt.asSeconds());
	sf::Vector2f netPos = m_NewPosition + (m_NewVelocity * updateInfo.dt.asSeconds());


	m_Position = (m_Position * 0.2f) + (netPos * 0.8f);
	m_Velocity = m_NewVelocity;
	m_Rotation = m_NewRotation; //no dead reckoning here

	char m_HealthPercent = (int)((m_Health / PLAYER_MAX_HEALTH)*255);

	m_Sprite.setColor(sf::Color(255, m_HealthPercent, m_HealthPercent));
	m_Sprite.setPosition(m_Position);
	m_Sprite.setRotation(m_Rotation + 180.0f);

	//projectile collision handled here

}

bool jr::Player::segmentIntersectsRectangle(const sf::FloatRect& rect, const sf::Vector2f& a_p1, const sf::Vector2f& a_p2)
{
	// Find min and max X for the segment
	float minX = std::min(a_p1.x, a_p2.x);
	float maxX = std::max(a_p1.x, a_p2.x);

	// Find the intersection of the segment's and rectangle's x-projections
	if (maxX > rect.left + rect.width) {
		maxX = rect.left + rect.width;
	}

	if (minX < rect.left) {
		minX = rect.left;
	}

	// If Y-projections do not intersect then there's no intersection
	if (minX > maxX) { return false; }

	// Find corresponding min and max Y for min and max X we found before
	float minY = a_p1.y;
	float maxY = a_p2.y;

	float dx = a_p2.x - a_p1.x;
	if (std::abs(dx) > 0.0000001f)//safeguard for float weirdness
	{
		float k = (a_p2.y - a_p1.y) / dx;
		float b = a_p1.y - k * a_p1.x;
		minY = k * minX + b;
		maxY = k * maxX + b;
	}

	if (minY > maxY) {
		std::swap(minY, maxY);
	}

	// Find the intersection of the segment's and rectangle's y-projections
	if (maxY > rect.top + rect.height) {
		maxY = rect.top + rect.height;
	}

	if (minY < rect.top) {
		minY = rect.top;
	}

	// If Y-projections do not intersect then there's no intersection
	if (minY > maxY) { return false; }
	return true;
}
