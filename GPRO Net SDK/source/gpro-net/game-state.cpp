#include "..\..\include\gpro-net\game-state.h"

#include "gpro-net/resource-manager.h"
#include "gpro-net/player.h"

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
	while (m_GameWindow.isOpen())
	{
		update();
	}
}

void GameState::update()
{
	sf::Time dt = frameClock.restart();
	handleRemoteInput(); //could be on seperate thread?

	sf::Event event;
	while (m_GameWindow.pollEvent(event))
	{
		handleSFMLEvent(event);
	}

	m_GameWindow.clear(sf::Color::Black); //clear window with black first
	
	m_GameWindow.draw(m_BackgroundSprite);

	//Update Entities
	for (int layer = 0; layer < (int)Layers::LAYERCOUNT; ++layer)
	{
		for (int index = 0; index < m_EntityLayers[layer].size(); ++index)
		{
			jr::Entity* e = m_EntityLayers[layer][index];
			if (e == nullptr) continue;


			RakNet::RakNetGUID myID = m_Peer->GetMyGUID();
			EntityUpdateInfo eui;
			eui.isOwner = myID.systemIndex == e->m_OwnerAddress.systemIndex;
			eui.dt = dt;
			eui.window = &m_GameWindow;
			e->update(eui);

			//Each object can have its own outputs ready to be sent
			for (int j = 0; j < e->m_RemoteOutputCache.size(); ++j)
			{
				m_RemoteOutputEventCache.push_back(e->m_RemoteOutputCache[j]);
			}
			e->m_RemoteOutputCache.clear();

			//Draw Entities
			m_GameWindow.draw(e->m_Sprite);
			if (layer == (unsigned char)Layers::PLAYER)
			{
				if (jr::Player* entityAsPlayer = dynamic_cast<jr::Player*>(e))
				{
					m_GameWindow.draw(entityAsPlayer->m_ArmSprite);
				}
			}
			else if (layer == (unsigned char)Layers::PROJECTILE)
			{
				if (jr::Projectile* entityAsProjectile = dynamic_cast<jr::Projectile*>(e))
				{
					//run collision on players
				}
			}
			


			if (e->m_DeleteMe)
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
	m_GameWindow.display();

	//send output network messages
	handleRemoteOutput(); //could be on seperate thread?
}

void jr::GameState::handleSFMLEvent(sf::Event e)
{
	// "close requested" event: we close the window
	if (e.type == sf::Event::Closed)
	{
		m_GameWindow.close();
	}

}

GameState::GameState() : GameState(sf::VideoMode(800, 600))
{
	
}

GameState::GameState(sf::VideoMode videoMode) : m_GameWindow(videoMode, "r/Place")
{
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
		

		//Setup Game
		m_BackgroundTexture.create(GRID_SIZE, GRID_SIZE);
		sf::Image bImage = m_BackgroundTexture.copyToImage();
		for (unsigned short xPos = 0; xPos < GRID_SIZE; ++xPos)
		{
			for (unsigned short yPos = 0; yPos < GRID_SIZE; ++yPos)
			{
				bImage.setPixel(xPos, yPos, sf::Color::White);
			}
		}

		m_BackgroundTexture.loadFromImage(bImage);
		m_BackgroundSprite.setScale(sf::Vector2f(16.0f, 16.0f));
		m_BackgroundSprite.setTexture(m_BackgroundTexture);
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

		jr::ResourceManager::cleanupSingleton();
	}
}
