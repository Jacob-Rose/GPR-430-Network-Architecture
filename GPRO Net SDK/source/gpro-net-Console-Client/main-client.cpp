/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	main-client.c/.cpp
	Main source for console client application.
*/

#include "gpro-net/gpro-net.h"

#include "client-game-state.h"

#define GAME_CLIENT 1

int main(void)
{
	//for us


	//NetworkState ns[1] = { 0 };

	const unsigned short SERVER_PORT = 7777;
	const char* SERVER_IP = "172.16.2.51"; //update every time

	jr::ClientGameState* gs = new jr::ClientGameState();
	
	gs->init();
	gs->connect(SERVER_IP, SERVER_PORT);

	gs->runGameLoop();

	delete gs;


	return 0;
}
