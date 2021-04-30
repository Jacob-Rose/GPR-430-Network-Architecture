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

private:
	sf::View m_PlayerView;
	jr::Player* m_LocalPlayer = nullptr;
	RakNet::RakNetGUID m_ServerGUID;

public:
	ClientGameState();

	virtual void init() override;
	virtual void connect(std::string ip, short port);
	virtual void cleanup() override;

	void update() override;
	void handleSFMLEvent(sf::Event e) override;
	
	void handleRemoteOutput() override;
	void handleRemoteInput() override;



};

#endif