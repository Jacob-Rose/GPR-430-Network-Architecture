#include "..\..\include\gpro-net\game-state.h"

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
	handleRemoteInput();

	sf::Event event;
	while (m_GameWindow.pollEvent(event))
	{
		// "close requested" event: we close the window
		if (event.type == sf::Event::Closed)
		{
			m_GameWindow.close();
		}
	}

	m_GameWindow.clear(sf::Color::Black); //clear window with black first



	m_BackgroundTexture.update(m_BackgroundImage);

	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(m_BackgroundTexture);

	m_GameWindow.draw(backgroundSprite);

	
	//Draw Entities
	for (int i = 0; i < m_NetworkEntities.size(); i++)
	{
		m_GameWindow.draw(m_NetworkEntities[i]->m_Sprite);
	}

	m_GameWindow.display();
}

jr::Entity* jr::GameState::getEntityById(int netID)
{
	for (int i = 0; i < m_NetworkEntities.size(); i++)
	{
		if (m_NetworkEntities[i]->m_NetID == netID)
		{
			return m_NetworkEntities[i];
		}
	}
	return nullptr;
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
		m_Peer = RakNet::RakPeerInterface::GetInstance();
		m_BackgroundImage.create(200, 200);
		m_BackgroundTexture.create(200, 200);
	}
}

void GameState::cleanup()
{
	if (m_IsInit)
	{
		RakNet::RakPeerInterface::DestroyInstance(m_Peer);

	}
}
