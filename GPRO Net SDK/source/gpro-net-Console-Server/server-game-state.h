#ifndef SERVER_GAME_WINDOW_H
#define SERVER_GAME_WINDOW_H

#include "gpro-net/game-state.h"

namespace jr
{
	class ServerGameState;
}


class ServerGameState : public jr::GameState
{
protected:
	const unsigned short MAX_CLIENTS = 10;
	const unsigned short SERVER_PORT = 7777;

	std::map<RakNet::RakNetGUID, jr::Player*> m_Players;
	std::vector<unsigned char> m_FreeNetworkIndexes[(int)Layers::LAYERCOUNT];

public:
	ServerGameState();


	void update() override;
	void handleRemoteOutput() override;
	void handleRemoteInput() override;

	void createPlayerForConn(RakNet::RakNetGUID conn);
	void deletePlayerForConn(RakNet::RakNetGUID conn);
	void deleteNetworkObject(jr::NetID netID);

	void createBulletRequested(NetworkObjectRequestCreateMessage* msg);

	virtual void init() override;
	virtual void cleanup() override;
};


#endif