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
	sf::Time dt = frameClock.restart();
	handleRemoteInput(); //could be on seperate thread?

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
	//Update Entities
	for (int i = 0; i < m_NetworkEntities.size(); i++)
	{
		RakNet::RakNetGUID myID = m_Peer->GetMyGUID();
		EntityUpdateInfo eui;
		eui.isOwner = myID.systemIndex == m_NetworkEntities[i]->m_OwnerAddress.systemIndex;
		eui.dt = dt;
		eui.window = &m_GameWindow;
		m_NetworkEntities[i]->Update(eui);
	}


	
	//Draw Entities
	for (int i = 0; i < m_NetworkEntities.size(); i++)
	{
		m_GameWindow.draw(m_NetworkEntities[i]->m_Sprite);

		if (m_NetworkEntities[i]->m_OwnerAddress.systemIndex == m_Peer->GetMyGUID().systemIndex)
		{
			NetworkObjectUpdateMessage* msg = new NetworkObjectUpdateMessage();
			msg->objectId = m_NetworkEntities[i]->m_NetID;
			msg->newPos[0] = m_NetworkEntities[i]->m_Position.x;
			msg->newPos[1] = m_NetworkEntities[i]->m_Position.y;
			msg->newRot = m_NetworkEntities[i]->m_Rotation;
			m_RemoteOutputEventCache.push_back(msg);
		}
	}

	//send buffer to monitor
	m_GameWindow.display();

	//send output network messages
	handleRemoteOutput(); //could be on seperate thread?
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
