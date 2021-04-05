#include "server-game-state.h"
#include "gpro-net/player.h"
#include <iostream>

ServerGameState::ServerGameState() : GameState()
{
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	m_Peer->Startup(MAX_CLIENTS, &sd, 1);
	m_Peer->SetMaximumIncomingConnections(MAX_CLIENTS);
}

void ServerGameState::update()
{
	for (int i = 0; i < m_RemoteInputEventCache.size(); ++i)
	{
		if (NotificationMessage* msg = dynamic_cast<NotificationMessage*>(m_RemoteInputEventCache[i]))
		{
			RakNet::MessageID id = msg->getMessageID();
			switch (id)
			{
			case ID_NEW_INCOMING_CONNECTION:
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
			{
				//todo player joined, make them join lobby
				//m_LobbyPlayers.push_back(msg->m_Sender);

				//Once they join a lobby send PlayerActiveOrderMessage to check if player or spectator
				//spawn player
				jr::Player* player = new jr::Player();
				player->m_OwnerAddress = msg->m_Sender;
				m_NetworkEntities.push_back(player); //we store in both because update uses m_NetworkObject, but finding players is easier in a map :)
				m_Players[msg->m_Sender] = player;

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
				updateMsg->newPos[0] = 0; //xpos
				updateMsg->newPos[1] = 0; //ypos
				updateMsg->newRot = 0.0f; //rot
				m_RemoteOutputEventCache.push_back(updateMsg);

				std::cout << "Player Connected \n";

				break;
			}
			case ID_CONNECTION_LOST:
			case ID_DISCONNECTION_NOTIFICATION:
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			case ID_REMOTE_CONNECTION_LOST:
			{
				std::map<RakNet::RakNetGUID, jr::Player*>::iterator pr_it = m_Players.find(msg->m_Sender);
				std::vector<jr::Entity*>::iterator no_it = std::find(m_NetworkEntities.begin(), m_NetworkEntities.end(), pr_it->second);

				NetworkObjectDestroyMessage* destroyMsg = new NetworkObjectDestroyMessage();
				destroyMsg->objectId = pr_it->second->m_NetID;
				m_RemoteOutputEventCache.push_back(destroyMsg);

				m_Players.erase(pr_it);
				m_NetworkEntities.erase(no_it);

				break;
			}
			}
		}
		else if (NetworkObjectUpdateMessage* msg = dynamic_cast<NetworkObjectUpdateMessage*>(m_RemoteInputEventCache[i]))
		{
			//todo update and pass on
		}
		delete m_RemoteInputEventCache[i];
	}
	m_RemoteInputEventCache.clear();


	GameState::update();
}

void ServerGameState::handleRemoteOutput()
{
	RakNet::BitStream bs;
	NetworkMessage::CreatePacketHeader(&bs, (int)m_RemoteOutputEventCache.size());
	for (int i = 0; i < m_RemoteOutputEventCache.size(); ++i)
	{
		m_RemoteOutputEventCache[i]->WritePacketBitstream(&bs);

		delete m_RemoteOutputEventCache[i];
	}
	m_Peer->Send(&bs, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); //this needs to have more control per message. but ill do this later
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
