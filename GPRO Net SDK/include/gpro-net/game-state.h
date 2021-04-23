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
#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "shared-net.h"
#include "projectile.h"

namespace jr
{
	class GameState;
	struct EntityLayer;

}


class jr::GameState
{
protected:

	const static short GRID_SIZE = 128;

	enum class Layers
	{
		WORLD_BASE = 0,
		ENVIORMENT, //obstacles
		PLAYER,
		ENEMY,
		PROJECTILE,
		LAYERCOUNT
	};

	const unsigned short LAYER_SIZES[(int)Layers::LAYERCOUNT] = { 
		1 /*The main world*/, 
		64 /*64 obstacles*/, 
		8 /*8 players*/, 
		24 /*24 Enemies*/, 
		127 /*127 projectiles*/ 
	};


	//||||||||||||| NETWORKING ||||||||||||
	RakNet::RakPeerInterface* m_Peer;
	std::vector<NetworkMessage*> m_RemoteInputEventCache; //filed in input
	std::vector<NetworkMessage*> m_RemoteOutputEventCache; //filled in update

	//||||||||||||| SFML |||||||||||||||||||
	sf::RenderWindow m_GameWindow;

	sf::Texture m_BackgroundTexture; //synced on network, thus doesnt use resource manager
	sf::Sprite m_BackgroundSprite;

	sf::Clock frameClock; //to get dt

	//State Information
	bool m_IsInit = false;
	

	virtual void handleRemoteInput(); //decypher packets and store in m_InputEventCache

	virtual void update();
	virtual void handleSFMLEvent(sf::Event e);

	virtual void handleRemoteOutput() = 0; //send out all packets needed in the m_OutputEventCache

	std::vector<jr::Entity*> m_EntityLayers[(int)Layers::LAYERCOUNT]; //layers so objects have draw order + for collisions
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