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
	ClientGameState(std::string ip, short port);

	virtual void init() override;
	virtual void cleanup() override;

	void update() override;
	
	void handleRemoteOutput() override;

	///
	/// Used only when a message from the server is received
	/// NOTE: cannot create objects on the server
	/// 
	/// Returns -1 if error, 1 if success
	///
	template <class ObjectType>
	int createNetworkObject(int index);
};

#endif

template<class ObjectType>
inline int jr::ClientGameState::createNetworkObject(int index)
{
	if (m_NetworkObjects.size() < index)
	{
		m_NetworkObjects.resize(((index % GameStateSettings::NETWORK_OBJECT_ARRAY_INCREMENT_AMOUNT) + 1) * GameStateSettings::NETWORK_OBJECT_ARRAY_INCREMENT_AMOUNT));
	}


	return 0;
}
