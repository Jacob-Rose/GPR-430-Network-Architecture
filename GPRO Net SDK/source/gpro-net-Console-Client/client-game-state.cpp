#include "client-game-state.h"

using namespace jr;

ClientGameState::ClientGameState(std::string ip, short port) : GameState()
{
	m_Peer->Connect(ip.c_str(), port, 0, 0);
}

void jr::ClientGameState::init()
{
	if (!m_IsInit)
	{
		GameState::init();
		m_IsInit = true;
	}
}

void jr::ClientGameState::cleanup()
{
	if (m_IsInit)
	{
		GameState::cleanup();
		m_IsInit = false;
	}
}

void ClientGameState::update()
{
	GameState::update();
}

void ClientGameState::handleRemoteOutput()
{
}
