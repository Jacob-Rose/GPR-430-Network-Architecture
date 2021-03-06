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


#include <string>
#include <iostream>
#define GAME_CLIENT 1

int main(void)
{
	const bool USE_LOCAL_NO_INPUT = true;
	const unsigned short SERVER_PORT = 7777;
	const char* SERVER_IP = "localhost"; //update every time

	jr::ClientGameState* gs = new jr::ClientGameState();


	std::string ip;

	if (USE_LOCAL_NO_INPUT)
	{
		ip = SERVER_IP;
	}
	else
	{
		std::cout << "Enter server IP: ";
		std::cin >> ip;
	}
	
	gs->init();
	gs->connect(ip.c_str(), SERVER_PORT);

	gs->runGameLoop();




	delete gs;
	return 0;
}
