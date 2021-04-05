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
	std::vector<int> m_FreeNetworkIndexes = std::vector<int>();

public:
	ServerGameState();


	void update() override;

	void handleRemoteOutput() override;

	template <class ObjectType>
	int createNetworkObject();

	virtual void init() override;
	virtual void cleanup() override;
};


#endif

template<class ObjectType>
inline int ServerGameState::createNetworkObject()
{
	//assert(std::is_base_of<ObjectType, jr::NetworkObject>::value)
	return 0;
}
