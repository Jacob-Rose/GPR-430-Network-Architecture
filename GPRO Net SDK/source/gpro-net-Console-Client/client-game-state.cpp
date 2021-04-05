#include "client-game-state.h"

using namespace jr;

ClientGameState::ClientGameState(std::string ip, short port) : GameState()
{
	m_Peer->Connect(ip.c_str(), port, 0, 0);
}

void jr::ClientGameState::init()
{
	if (!m_IsInit)
	{
		GameState::init();
		m_IsInit = true;
	}
}

void jr::ClientGameState::cleanup()
{
	if (m_IsInit)
	{
		GameState::cleanup();
		m_IsInit = false;
	}
}

void ClientGameState::update()
{
	for (int i = 0; i < m_RemoteInputEventCache.size(); ++i)
	{
		if (NotificationMessage* msg = dynamic_cast<NotificationMessage*>(m_RemoteInputEventCache[i]))
		{
			RakNet::MessageID id = msg->getMessageID();
			/*
			switch (id)
			{
				
			}
			*/
		}
		else if (NetworkObjectUpdateMessage* msg = dynamic_cast<NetworkObjectUpdateMessage*>(m_RemoteInputEventCache[i]))
		{
			//todo update and pass on
			jr::Entity* e = getEntityById(msg->objectId);
			e->m_Position.x = msg->newPos[0];
			e->m_Position.y = msg->newPos[1];
			e->m_Rotation = msg->newRot;
		}
		else if (NetworkObjectCreateMessage* msg = dynamic_cast<NetworkObjectCreateMessage*>(m_RemoteInputEventCache[i]))
		{
			jr::Entity* newEntity;
			switch (msg->objectType)
			{
			case NetworkObjectID::PLAYER_OBJECT_ID:
				newEntity = new jr::Player();
				
				m_NetworkEntities.push_back(newEntity);
				break;
			case NetworkObjectID::ENEMY_OBJECT_ID:
				newEntity = new jr::Enemy();
				break;
			default:
				return;
			}
			newEntity->m_NetID = msg->objectId;
			
		}
		else if (NetworkObjectAuthorityChangeMessage* msg = dynamic_cast<NetworkObjectAuthorityChangeMessage*>(m_RemoteInputEventCache[i]))
		{
			jr::Entity* e = getEntityById(msg->objectId);
			e->m_OwnerAddress = msg->newAddress;
		}
		delete m_RemoteInputEventCache[i];
	}

	GameState::update();
}

void ClientGameState::handleRemoteOutput()
{
}
