#ifndef CLIENT_GAME_STATE_H
#define CLIENT_GAME_STATE_H

#include "gpro-net/game-state.h"

#include "gpro-net/player.h"

namespace jr
{
	class ClientGameState;
}


class jr::ClientGameState : public jr::GameState
{
protected:

public:
	ClientGameState();

	virtual void init() override;
	virtual void connect(std::string ip, short port);
	virtual void cleanup() override;

	void update() override;
	
	void handleRemoteOutput() override;

	RakNet::RakNetGUID m_ServerGUID;
	///
	/// Used only when a message from the server is received
	/// NOTE: cannot create objects on the server
	/// 
	/// Returns -1 if error, 1 if success
	///
};

#endif