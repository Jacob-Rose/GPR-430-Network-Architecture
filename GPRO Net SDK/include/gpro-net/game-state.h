#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H


//SFML
#include <SFML/Graphics.hpp>

#include <gpro-net/tilemap.hpp>

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
public:

	const static short GRID_SIZE = 128;

	enum class Layers //i love this, so useful
	{
		ENVIORMENT = 0, //World Tiles
		WALLS,
		PLAYER,
		ENEMY,
		PROJECTILE,
		LAYERCOUNT
	};
	const static int MAP_WIDTH = 12;
	const static int MAP_HEIGHT = 12;


	const unsigned short LAYER_SIZES[(int)Layers::LAYERCOUNT] = { 
		0, //enviorment is not preallocated (could be, but im lazy)
		0, //wall is not preallocated (could be, but im lazy)
		8 /*8 players*/, 
		24 /*24 Enemies*/, 
		127 /*127 projectiles*/ 
	};



	const char levelTilemap[MAP_WIDTH][MAP_HEIGHT] = 
	{
		1,1,1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,1,1,
	};



	//||||||||||||| NETWORKING ||||||||||||
	RakNet::RakPeerInterface* m_Peer;
	std::vector<NetworkMessage*> m_RemoteInputEventCache; //filed in input
	std::vector<NetworkMessage*> m_RemoteOutputEventCache; //filled in update

	//||||||||||||| SFML |||||||||||||||||||
	sf::RenderWindow* m_GameWindow;
	sf::TileMap m_BackgroundMap;

	sf::Clock frameClock; //to get dt

	//State Information
	bool m_IsInit = false;

	bool m_GameActive = true;
	

	virtual void handleRemoteInput(); //decypher packets and store in m_InputEventCache

	virtual void update();



	virtual void handleSFMLEvent(sf::Event e);

	virtual void handleRemoteOutput() = 0; //send out all packets needed in the m_OutputEventCache

	std::vector<jr::Entity*> m_EntityLayers[(int)Layers::LAYERCOUNT]; //layers so objects have draw order + for collisions
public:

	GameState(bool useWindow = true);
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