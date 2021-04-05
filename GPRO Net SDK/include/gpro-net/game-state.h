#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H


//SFML
#include <SFML/Graphics.hpp>

//Standard Library
#include <vector>

//RakNet
#include <RakNet/RakPeerInterface.h>

//In-House
#include "network-messages.h"
#include "player.h"
#include "enemy.h"

namespace jr
{
	class GameState;
}

class GameStateSettings
{
public:
	const static short NETWORK_OBJECT_START_SIZE = 8;
	const static short NETWORK_OBJECT_ARRAY_INCREMENT_AMOUNT = 8;
};

class jr::GameState
{
protected:
	//||||||||||||| NETWORKING ||||||||||||
	RakNet::RakPeerInterface* m_Peer;
	std::vector<NetworkMessage*> m_RemoteInputEventCache; //filed in input
	std::vector<NetworkMessage*> m_RemoteOutputEventCache; //filled in update

	//||||||||||||| SFML |||||||||||||||||||
	sf::RenderWindow m_GameWindow;

	sf::Image m_BackgroundImage; //synced on network
	sf::Texture m_BackgroundTexture; //loads from m_BackgroundImage

	//State Information
	bool m_IsInit = false;
	

	virtual void handleRemoteInput(); //decypher packets and store in m_InputEventCache

	virtual void update();

	virtual void handleRemoteOutput() = 0; //send out all packets needed in the m_OutputEventCache

	std::vector<jr::Entity*> m_NetworkEntities = std::vector<jr::Entity*>();

	jr::Entity* getEntityById(int netID);

public:

	GameState();
	GameState(sf::VideoMode videoMode);

	~GameState();

	//Note: Does not set m_IsInit as the extended parent will most likely be doing this
	virtual void init();
	virtual void cleanup();


	void runGameLoop();

	//Getters
	RakNet::RakPeerInterface* getPeer() { return m_Peer; }
};

#endif