#include "server-game-state.h"
#include "gpro-net/player.h"
#include "gpro-net/projectile.h"
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

				createPlayerForConn(msg->m_Sender);

				break; //switch break
			}
			case ID_CONNECTION_LOST:
			case ID_DISCONNECTION_NOTIFICATION:
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			case ID_REMOTE_CONNECTION_LOST:
			{
				printf("A client has disconnected.\n");
				deletePlayerForConn(msg->m_Sender);

				break; //switch break
			}
			}
		}
		else if (NetworkObjectUpdateMessage* msg = dynamic_cast<NetworkObjectUpdateMessage*>(m_RemoteInputEventCache[i]))
		{
			jr::Entity* e = m_EntityLayers[msg->m_NetID.layer][msg->m_NetID.id];
			if (e->m_OwnerAddress == msg->m_Sender)
			{
				e->m_Position.x = msg->newPos[0];
				e->m_Position.y = msg->newPos[1];
				e->m_Rotation = msg->newRot;

				m_RemoteOutputEventCache.push_back(msg);
				continue; //DO NOT DELETE ME

			}
		}
		else if (NetworkObjectDestroyMessage* msg = dynamic_cast<NetworkObjectDestroyMessage*>(m_RemoteInputEventCache[i]))
		{
			jr::Entity* e = m_EntityLayers[msg->m_NetID.layer][msg->m_NetID.id];
			if (e->m_OwnerAddress == msg->m_Sender)
			{
				m_FreeNetworkIndexes[e->m_NetID.layer].push_back(e->m_NetID.id);
				delete e;
				m_EntityLayers[msg->m_NetID.layer][msg->m_NetID.id] = nullptr;
				m_RemoteOutputEventCache.push_back(msg);
				continue; //DO NOT DELETE ME
			}
		}
		else if (NetworkObjectRequestCreateMessage* msg = dynamic_cast<NetworkObjectRequestCreateMessage*>(m_RemoteInputEventCache[i]))
		{
			if (msg->objectType == NetworkObjectID::BULLET_OBJECT_ID) //only one we allow players to spawn
			{
				createBulletRequested(msg);
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

void ServerGameState::createPlayerForConn(RakNet::RakNetGUID conn)
{
	jr::Player* player = new jr::Player();

	unsigned char objectID = m_FreeNetworkIndexes[(int)Layers::PLAYER].back();
	m_FreeNetworkIndexes[(int)Layers::PLAYER].pop_back();

	player->m_NetID.id = objectID;
	player->m_NetID.layer = (unsigned char)Layers::PLAYER;
	player->m_OwnerAddress = conn;
	player->m_Position.x = 100;
	player->m_Position.y = 100;
	player->m_Rotation = 0.0f;

	m_EntityLayers[(int)Layers::PLAYER][objectID] = player; //we store in both because update uses m_NetworkObject, but finding players is easier in a map :)
	m_Players[conn] = player; //kind of unnecessary here and in the game loop, but helps with finding and deleting later, could be removed if i cared enough :)

	//now send spawn message
	NetworkObjectCreateMessage* createMsg = new NetworkObjectCreateMessage();
	createMsg->m_NetID.layer = (unsigned char)Layers::PLAYER;
	createMsg->m_NetID.id = objectID;
	createMsg->objectType = NetworkObjectID::PLAYER_OBJECT_ID;

	//now send authority message
	NetworkObjectAuthorityChangeMessage* authMsg = new NetworkObjectAuthorityChangeMessage();
	authMsg->m_NetID.id = objectID; //gotten above
	authMsg->m_NetID.layer = (unsigned char)Layers::PLAYER; //gotten above
	authMsg->newAddress = player->m_OwnerAddress;
	
	//and finally send update information
	NetworkObjectUpdateMessage* updateMsg = new NetworkObjectUpdateMessage();
	updateMsg->m_NetID.id = objectID;
	updateMsg->m_NetID.layer = (unsigned char)Layers::PLAYER;
	updateMsg->newPos[0] = player->m_Position.x; //xpos
	updateMsg->newPos[1] = player->m_Position.y; //ypos
	updateMsg->newRot = player->m_Rotation; //rot
	

	//Add msgs to queue
	m_RemoteOutputEventCache.push_back(createMsg);
	m_RemoteOutputEventCache.push_back(authMsg);
	m_RemoteOutputEventCache.push_back(updateMsg);
}

void ServerGameState::deletePlayerForConn(RakNet::RakNetGUID conn)
{
	std::map<RakNet::RakNetGUID, jr::Player*>::iterator pr_it = m_Players.find(conn);
	deleteNetworkObject(pr_it->second->m_NetID);
	m_Players.erase(pr_it);
}

void ServerGameState::deleteNetworkObject(jr::NetID netID)
{
	jr::Entity* e = m_EntityLayers[netID.layer][netID.id];

	NetworkObjectDestroyMessage* destroyMsg = new NetworkObjectDestroyMessage();
	destroyMsg->m_NetID = e->m_NetID;

	m_RemoteOutputEventCache.push_back(destroyMsg);

	m_FreeNetworkIndexes[netID.layer].push_back(e->m_NetID.id);
	m_EntityLayers[netID.layer][netID.id] = nullptr;

	delete e;
}

void ServerGameState::createBulletRequested(NetworkObjectRequestCreateMessage* msg)
{
	jr::Projectile* projectile = new jr::Projectile();
	unsigned char objectID = m_FreeNetworkIndexes[(unsigned char)Layers::PROJECTILE].back();
	m_FreeNetworkIndexes[(unsigned char)Layers::PROJECTILE].pop_back();


	projectile->m_NetID.id = objectID;
	projectile->m_NetID.layer = (unsigned char)Layers::PROJECTILE;
	projectile->m_OwnerAddress = msg->m_Sender;
	projectile->m_Position.x = msg->objectPos[0];
	projectile->m_Position.y = msg->objectPos[1];
	projectile->m_Rotation = msg->objectRot;

	m_EntityLayers[projectile->m_NetID.layer][projectile->m_NetID.id] = projectile;


	//now send spawn message
	NetworkObjectCreateMessage* createMsg = new NetworkObjectCreateMessage();

	createMsg->m_NetID.id = objectID;
	createMsg->m_NetID.layer = (unsigned char)Layers::PROJECTILE;
	createMsg->objectType = NetworkObjectID::BULLET_OBJECT_ID;

	//now send authority message
	NetworkObjectAuthorityChangeMessage* authMsg = new NetworkObjectAuthorityChangeMessage();
	authMsg->m_NetID.id = objectID;
	authMsg->m_NetID.layer = (unsigned char)Layers::PROJECTILE;
	authMsg->newAddress = projectile->m_OwnerAddress;

	//and finally send update information
	NetworkObjectUpdateMessage* updateMsg = new NetworkObjectUpdateMessage();
	updateMsg->newPos[0] = projectile->m_Position.x; //xpos
	updateMsg->newPos[1] = projectile->m_Position.y; //ypos
	updateMsg->newRot = projectile->m_Rotation; //rot
	updateMsg->m_NetID.id = objectID;
	updateMsg->m_NetID.layer = (unsigned char)Layers::PROJECTILE;

	//Add msgs to queue
	m_RemoteOutputEventCache.push_back(createMsg);
	m_RemoteOutputEventCache.push_back(authMsg);
	m_RemoteOutputEventCache.push_back(updateMsg);

}

void ServerGameState::init()
{
	if (!m_IsInit)
	{
		GameState::init();

		RakNet::SocketDescriptor sd(SERVER_PORT, 0);
		m_Peer->Startup(MAX_CLIENTS, &sd, 1);
		m_Peer->SetMaximumIncomingConnections(MAX_CLIENTS);

		
		
		for (unsigned char layer = 0; layer < (int)Layers::LAYERCOUNT; ++layer) //todo remove magic number, but at the same time who really cares
		{
			for (short id = LAYER_SIZES[layer]; id >= 0; --id)
			{
				m_FreeNetworkIndexes[layer].push_back((unsigned char)id);
			}
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
