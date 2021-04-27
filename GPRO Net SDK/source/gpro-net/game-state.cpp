#include "..\..\include\gpro-net\game-state.h"

#include "gpro-net/resource-manager.h"
#include "gpro-net/player.h"
#include "gpro-net/tile.hpp"

using namespace jr; //jakerose namespace

void GameState::handleRemoteInput()
{
	RakNet::Packet* packet;
	for (packet = m_Peer->Receive(); packet; m_Peer->DeallocatePacket(packet), packet = m_Peer->Receive())
	{
		RakNet::BitStream bsIn(packet->data, packet->length, false);
		NetworkMessage::DecypherPacket(&bsIn, packet->guid, m_RemoteInputEventCache);
		//yup, thats it for the input step
	}
}

void GameState::runGameLoop()
{
	if (m_GameWindow != nullptr)
	{
		while (m_GameWindow->isOpen() && m_GameActive)
		{
			update();
		}
	}
	else
	{
		while (m_GameActive)
		{
			update();
		}
	}

}

void jr::GameState::handleRemoteOutput()
{
	for (int i = 0; i < m_RemoteOutputEventCache.size(); ++i)
	{
		RakNet::BitStream bs;
		m_RemoteOutputEventCache[i]->WritePacketBitstream(&bs);
		if (m_RemoteOutputEventCache[i]->m_Target == RakNet::UNASSIGNED_RAKNET_GUID)
		{
			if (m_RemoteOutputEventCache[i]->m_Sender == m_Peer->GetMyGUID())
			{
				m_Peer->Send(&bs, m_RemoteOutputEventCache[i]->m_Priority, m_RemoteOutputEventCache[i]->m_Reliablity, 0, RakNet::UNASSIGNED_RAKNET_GUID, true); //this needs to have more control per message. but ill do this later
			}
			else
			{
				m_Peer->Send(&bs, m_RemoteOutputEventCache[i]->m_Priority, m_RemoteOutputEventCache[i]->m_Reliablity, 0, m_RemoteOutputEventCache[i]->m_Sender, true); //this needs to have more control per message. but ill do this later
			}
			
		}
		else
		{
			m_Peer->Send(&bs, m_RemoteOutputEventCache[i]->m_Priority, m_RemoteOutputEventCache[i]->m_Reliablity, 0, m_RemoteOutputEventCache[i]->m_Target, false);
		}
		delete m_RemoteOutputEventCache[i];
	}
	m_RemoteOutputEventCache.clear();
}

jr::Player* jr::GameState::getPlayerForAddress(RakNet::RakNetGUID playerID)
{
	for (int i = 0; i < m_EntityLayers[(int)Layers::PLAYER].size(); ++i)
	{
		if (m_EntityLayers[(int)Layers::PLAYER][i]->m_OwnerAddress == playerID)
		{
			return static_cast<jr::Player*>(m_EntityLayers[(int)Layers::PLAYER][i]);
		}
	}
	return nullptr;
}

sf::Vector2f jr::GameState::getSizeOfWorldTile()
{
	return sf::Vector2f(m_EntityLayers[(int)Layers::ENVIORMENT][0]->m_Sprite.getGlobalBounds().width, m_EntityLayers[(int)Layers::ENVIORMENT][0]->m_Sprite.getGlobalBounds().height);
}

void GameState::update()
{
	sf::Time dt = frameClock.restart();
	handleRemoteInput(); //could be on seperate thread?

	sf::Event event;

	if (m_GameWindow != nullptr)
	{
		while (m_GameWindow->pollEvent(event))
		{
			handleSFMLEvent(event);
		}

		m_GameWindow->clear(sf::Color::Black); //clear window with black first

		m_GameWindow->draw(m_BackgroundMap);
	}
	
	EntityUpdateInfo eui; //used for all useful since a struct
	eui.dt = dt;
	eui.gameState = this;
	//Update Entities
	for (int layer = 0; layer < (int)Layers::LAYERCOUNT; ++layer)
	{
		for (int index = 0; index < m_EntityLayers[layer].size(); ++index)
		{
			jr::Entity* e = m_EntityLayers[layer][index];
			if (e == nullptr) continue;


			RakNet::RakNetGUID myID = m_Peer->GetMyGUID();
			eui.isOwner = myID == e->m_OwnerAddress;
			e->update(eui);

			//Each object can have its own outputs ready to be sent
			for (int j = 0; j < e->m_RemoteOutputCache.size(); ++j)
			{
				m_RemoteOutputEventCache.push_back(e->m_RemoteOutputCache[j]);
			}
			e->m_RemoteOutputCache.clear();

			//Draw Entities
			if (m_GameWindow != nullptr)
			{
				m_GameWindow->draw(e->m_Sprite);
			}
			


			if (e->m_OwnerAddress == m_Peer->GetMyGUID() && e->m_DeleteMe)
			{
				delete e;
				m_EntityLayers[layer][index] = nullptr;
				NetworkObjectDestroyMessage* msg = new NetworkObjectDestroyMessage();
				msg->m_NetID.layer = layer;
				msg->m_NetID.id = index;
				m_RemoteOutputEventCache.push_back(msg);
			}
		}
	}



	//send buffer to monitor
	if (m_GameWindow != nullptr)
	{
		m_GameWindow->display();
	}


	//send output network messages
	handleRemoteOutput(); //could be on seperate thread?
}

void jr::GameState::handleSFMLEvent(sf::Event e)
{
	//only called if window exist, no check necessary
	// "close requested" event: we close the window
	if (e.type == sf::Event::Closed)
	{
		m_GameWindow->close();
	}

}

GameState::GameState(bool useWindow)
{
	if (useWindow)
	{
		m_GameWindow = new sf::RenderWindow(sf::VideoMode(800, 600), "r/Place");
	}
	else
	{
		m_GameWindow = nullptr;
	}
	init();
}

GameState::GameState(sf::VideoMode videoMode)
{
	m_GameWindow = new sf::RenderWindow(videoMode, "r/Place");
	
	init();
}

GameState::~GameState()
{
	cleanup();
}

void GameState::init()
{
	if (!m_IsInit)
	{
		jr::ResourceManager::initSingleton();

		m_Peer = RakNet::RakPeerInterface::GetInstance();

		
		//Set up all layers (sets itself all up without any additional code
		for (unsigned char layer = 0; layer < (int)Layers::LAYERCOUNT; ++layer)
		{
			m_EntityLayers[layer] = std::vector<jr::Entity*>(LAYER_SIZES[layer], nullptr);
		}

		int envCounter = 0; //idc about indexes here, just increment when a non-wall added
		int wallCounter = 0; //idc about indexes here, just increment when a non-wall added
		for (unsigned char x = 0; x < MAP_WIDTH; ++x)
		{
			for (unsigned char y = 0; y < MAP_HEIGHT; ++y)
			{
				if (levelTilemap[x][y] == 1)
				{
					m_EntityLayers[(int)Layers::WALLS].push_back(new jr::TileEntity(levelTilemap[x][y], sf::Vector2i(x, y)));
					++wallCounter;
				}
				else
				{
					m_EntityLayers[(int)Layers::ENVIORMENT].push_back(new jr::TileEntity(levelTilemap[x][y], sf::Vector2i(x, y)));
					++envCounter;
				}
			}
		}


		//Setup Game
		//m_BackgroundMap.load("resources/textures/kenny/topdown-shooter/Tilesheet/tilesheet_complete.png", sf::Vector2u(64, 64), levelData, MAP_WIDTH, MAP_HEIGHT);

		//m_BackgroundTexture.loadFromImage(bImage);
		//m_BackgroundSprite.setScale(sf::Vector2f(16.0f, 16.0f));
		//m_BackgroundSprite.setTexture(m_BackgroundTexture);
	}
}

void GameState::cleanup()
{
	if (m_IsInit)
	{
		//Cleanup game
		for (unsigned char layer = 0; layer < (int)Layers::LAYERCOUNT; ++layer)
		{
			for (unsigned char itemID = 0; itemID < m_EntityLayers[layer].size(); ++itemID)
			{
				if (m_EntityLayers[layer][itemID] != nullptr)
				{
					delete m_EntityLayers[layer][itemID];
				}
			}
			m_EntityLayers[layer].clear();
		}


		m_Peer->Shutdown(0);
		RakNet::RakPeerInterface::DestroyInstance(m_Peer);
		delete m_GameWindow;

		jr::ResourceManager::cleanupSingleton();
	}
}
