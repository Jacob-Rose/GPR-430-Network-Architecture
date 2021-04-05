#include "server-game-state.h"
#include "gpro-net/player.h"
#include <iostream>

ServerGameState::ServerGameState() : GameState()
{
	m_GameWindow.setTitle("r/Place Server");
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
				printf("A client has connected.\n");

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

				player->m_NetID = objectID;

				//now send authority message
				NetworkObjectAuthorityChangeMessage* authMsg = new NetworkObjectAuthorityChangeMessage();
				authMsg->objectId = objectID; //gotten above
				authMsg->newAddress = msg->m_Sender;
				m_RemoteOutputEventCache.push_back(authMsg);

				player->m_OwnerAddress = msg->m_Sender;

				//and finally send update information
				NetworkObjectUpdateMessage* updateMsg = new NetworkObjectUpdateMessage();
				updateMsg->newPos[0] = 0; //xpos
				updateMsg->newPos[1] = 0; //ypos
				updateMsg->newRot = 0.0f; //rot
				updateMsg->objectId = objectID;
				m_RemoteOutputEventCache.push_back(updateMsg);

				player->m_Position.x = 0;
				player->m_Position.y = 0;
				player->m_Rotation = 0.0f;

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
				delete pr_it->second;

				break;
			}
			}
		}
		else if (NetworkObjectUpdateMessage* msg = dynamic_cast<NetworkObjectUpdateMessage*>(m_RemoteInputEventCache[i]))
		{
			//todo update and pass on
			jr::Entity* e = getEntityById(msg->objectId);
			if (e->m_OwnerAddress.systemIndex == msg->m_Sender.systemIndex)
			{
				e->m_Position.x = msg->newPos[0];
				e->m_Position.y = msg->newPos[1];
				e->m_Rotation = msg->newRot;

				m_RemoteOutputEventCache.push_back(msg);
				continue; //DO NOT DELETE

			}
			
		}
		delete m_RemoteInputEventCache[i];
	}
	m_RemoteInputEventCache.clear();


	GameState::update();
}

void ServerGameState::handleRemoteOutput()
{
	for (int i = 0; i < m_RemoteOutputEventCache.size(); ++i)
	{
		RakNet::BitStream bs;
		m_RemoteOutputEventCache[i]->WritePacketBitstream(&bs);
		m_Peer->Send(&bs, m_RemoteOutputEventCache[i]->m_Priority, m_RemoteOutputEventCache[i]->m_Reliablity, 0, m_RemoteOutputEventCache[i]->m_Sender, true); //this needs to have more control per message. but ill do this later
		
		delete m_RemoteOutputEventCache[i];
	}
	m_RemoteOutputEventCache.clear();
	
}

void ServerGameState::init()
{
	if (!m_IsInit)
	{
		GameState::init();

		RakNet::SocketDescriptor sd(SERVER_PORT, 0);
		m_Peer->Startup(MAX_CLIENTS, &sd, 1);
		m_Peer->SetMaximumIncomingConnections(MAX_CLIENTS);
		for (int i = 0; i < 100; i++) //todo remove magic number, but at the same time who really cares
		{
			m_FreeNetworkIndexes.push_back(i);
		}
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
