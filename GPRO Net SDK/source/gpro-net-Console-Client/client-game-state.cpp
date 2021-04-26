#include "client-game-state.h"
#include "gpro-net/projectile.h"

using namespace jr;

ClientGameState::ClientGameState() : GameState(), m_PlayerView(sf::FloatRect(0, 0, 800, 600))
{
	
}

void jr::ClientGameState::init()
{
	if (!m_IsInit)
	{
		GameState::init();
		m_IsInit = true;
		m_PlayerView.setSize(sf::Vector2f(m_GameWindow->getSize()));
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
		if (m_RemoteInputEventCache[i] == nullptr)
		{
			continue;
		}
		if (NotificationMessage* msg = dynamic_cast<NotificationMessage*>(m_RemoteInputEventCache[i]))
		{
			RakNet::MessageID id = msg->getMessageID();
			
			switch (id)
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");
				m_ServerGUID = msg->m_Sender;
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
			jr::Entity* e = m_EntityLayers[msg->m_NetID.layer][msg->m_NetID.id];
			e->m_NewPosition.x = msg->newPos[0];
			e->m_NewPosition.y = msg->newPos[1];
			e->m_NewRotation = msg->newRot;
			if (msg->hard)
			{
				e->m_Position.x = msg->newPos[0];
				e->m_Position.y = msg->newPos[1];
				e->m_Rotation = msg->newRot;
			}
		}
		else if (NetworkObjectCreateMessage* msg = dynamic_cast<NetworkObjectCreateMessage*>(m_RemoteInputEventCache[i]))
		{
			jr::Entity* newEntity;
			switch (msg->objectType)
			{
			case NetworkObjectID::PLAYER_OBJECT_ID:
				newEntity = new jr::Player();
				
				break;
			case NetworkObjectID::ENEMY_OBJECT_ID:
				newEntity = new jr::Enemy();
				break;

			case NetworkObjectID::BULLET_OBJECT_ID:
				newEntity = new jr::Projectile();
				break;
			default:
				continue;
			}


			m_EntityLayers[msg->m_NetID.layer][msg->m_NetID.id] = newEntity;
			newEntity->m_NetID = msg->m_NetID;
			
		}
		else if (NetworkObjectDestroyMessage* msg = dynamic_cast<NetworkObjectDestroyMessage*>(m_RemoteInputEventCache[i]))
		{
			delete m_EntityLayers[msg->m_NetID.layer][msg->m_NetID.id];
			m_EntityLayers[msg->m_NetID.layer][msg->m_NetID.id] = nullptr;
		}
		else if (NetworkObjectAuthorityChangeMessage* msg = dynamic_cast<NetworkObjectAuthorityChangeMessage*>(m_RemoteInputEventCache[i]))
		{
			jr::Entity* e = m_EntityLayers[msg->m_NetID.layer][msg->m_NetID.id];
			e->m_OwnerAddress = msg->newAddress;

			if (jr::Player* player = dynamic_cast<jr::Player*>(e))
			{
				if (msg->newAddress == m_Peer->GetMyGUID())
				{
					m_LocalPlayer = player;
				}
			}
		}

		delete m_RemoteInputEventCache[i];
		m_RemoteInputEventCache[i] = nullptr;
	}
	m_RemoteInputEventCache.clear();

	if (m_LocalPlayer != nullptr)
	{
		sf::Vector2f newPos = m_PlayerView.getCenter() * 0.6f + m_LocalPlayer->m_Position * 0.4f;
		m_PlayerView.setCenter(newPos);
	}
	m_GameWindow->setView(m_PlayerView);


	
	GameState::update();
}

void jr::ClientGameState::handleSFMLEvent(sf::Event e)
{
	GameState::handleSFMLEvent(e);
	// "close requested" event: we close the window
	if (e.type == sf::Event::MouseButtonPressed)
	{
		if (e.mouseButton.button == sf::Mouse::Button::Left)
		{ 
			/*
			sf::Vector2f mouseCoord = m_GameWindow.mapPixelToCoords(sf::Vector2i(e.mouseButton.x, e.mouseButton.y) / (int)(m_BackgroundSprite.getTexture()->getSize().x * m_BackgroundSprite.getGlobalBounds().width);
			sf::Vector2i pixelLoc;
			pixelLoc.x = 
			m_BackgroundSprite.getGlobalBounds();
			*/
		}
	}
}

void ClientGameState::handleRemoteOutput()
{
	for (int i = 0; i < m_RemoteOutputEventCache.size(); ++i)
	{
		RakNet::BitStream bs;
		m_RemoteOutputEventCache[i]->WritePacketBitstream(&bs);
		m_Peer->Send(&bs, m_RemoteOutputEventCache[i]->m_Priority, m_RemoteOutputEventCache[i]->m_Reliablity, 0, m_ServerGUID, false); //this needs to have more control per message. but ill do this later

		delete m_RemoteOutputEventCache[i];
	}
	m_RemoteOutputEventCache.clear();
}
