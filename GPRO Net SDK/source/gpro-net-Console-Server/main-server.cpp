/*
   Copyright 2021 Jacob Rose

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

//SFML
#include <SFML/Window.hpp>

//std
#include <stdio.h>
#include <string.h>
#include <vector>
#include <future>
#include <limits>
#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <map>

//RakNet
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/GetTime.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID

#include "gpro-net/shared-net.h"
#include "gpro-net/network-messages.h"
#include "server-game-state.h"


int main(int argc, char* argv[])
{
	//I refuse to write anything logical in main
	ServerGameState* gs = new ServerGameState();

	gs->init();

	gs->runGameLoop();

	gs->cleanup();

	delete gs;
	gs = nullptr;

	return 0;
}