#include "client-game-state.h"

using namespace jr;

ClientGameState::ClientGameState() : GameState()
{
	
}

void jr::ClientGameState::init()
{
	if (!m_IsInit)
	{
		GameState::init();
		
		
		m_IsInit = true;
	}
}

void jr::ClientGameState::connect(std::string ip, short port)
{
	RakNet::SocketDescriptor sd;
	m_Peer->Startup(1, &sd, 1);
	m_Peer->Connect(ip.c_str(), port, 0, 0);
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
			
			switch (id)
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");
			}
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			case ID_REMOTE_CONNECTION_LOST:

				//a player disconnected (not us)
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full, cannot join.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("We have disconnected.\n");
				break;
			case ID_CONNECTION_LOST:
				printf("We lost connection.\n");
				break;
			}
			
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
	m_RemoteInputEventCache.clear();
	GameState::update();
}

void ClientGameState::handleRemoteOutput()
{
}
