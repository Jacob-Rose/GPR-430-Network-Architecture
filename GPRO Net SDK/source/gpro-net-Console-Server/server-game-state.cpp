#include "server-game-state.h"
#include "gpro-net/player.h"

ServerGameState::ServerGameState() : GameState()
{
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	m_Peer->Startup(MAX_CLIENTS, &sd, 1);
	m_Peer->SetMaximumIncomingConnections(MAX_CLIENTS);
}

void ServerGameState::update()
{
	for (int i = 0; i < m_RemoteInputEventCache.size(); i++)
	{
		if (NotificationMessage* msg = dynamic_cast<NotificationMessage*>(m_RemoteInputEventCache[i]))
		{
			switch (msg->m_MessageID)
			{
			case ID_NEW_INCOMING_CONNECTION:
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
			{
				//todo player joined, make them join lobby
				//m_LobbyPlayers.push_back(msg->m_Sender);

				//Once they join a lobby send PlayerActiveOrderMessage to check if player or spectator
				//spawn player
				jr::Entity* player = new jr::Player();
				player->m_OwnerAddress = msg->m_Sender;
				m_NetworkObjects.push_back(player);

				//now send spawn message
				NetworkObjectCreateMessage* createMsg = new NetworkObjectCreateMessage();
				short objectID = m_FreeNetworkIndexes.back();
				m_FreeNetworkIndexes.pop_back();
				createMsg->objectId = objectID;
				createMsg->objectType = NetworkObjectID::PLAYER_OBJECT_ID;
				m_RemoteOutputEventCache.push_back(createMsg);

				//now send authority message
				NetworkObjectAuthorityChangeMessage* authMsg = new NetworkObjectAuthorityChangeMessage();
				authMsg->objectId = objectID; //gotten above
				authMsg->newAddress = msg->m_Sender;
				m_RemoteOutputEventCache.push_back(authMsg);

				//and finally send update information
				NetworkObjectUpdateMessage* updateMsg = new NetworkObjectUpdateMessage();
				break;
			}
			case ID_CONNECTION_LOST:
			case ID_DISCONNECTION_NOTIFICATION:
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			case ID_REMOTE_CONNECTION_LOST:
			{
				//todo player disconnected
				break;
			}
			}
		}
		delete m_RemoteInputEventCache[i];
	}
	m_RemoteInputEventCache.clear();


	GameState::update();
}

void ServerGameState::handleRemoteOutput()
{
}

void ServerGameState::init()
{
	if (!m_IsInit)
	{
		GameState::init();
		m_IsInit = true;
	}
}

void ServerGameState::cleanup()
{
	if (m_IsInit)
	{
		GameState::cleanup();
		m_IsInit = false;
	}
}
