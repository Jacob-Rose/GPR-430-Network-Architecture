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

//STD
#include <stdio.h>
#include <string.h>
#include <vector>
#include <future>
#include <limits>
#include <iostream>
#include <list>
#include <map>
#include <numeric>

//RakNet
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/GetTime.h>
#include <RakNet/StringCompressor.h>

#include "gpro-net/shared-net.h"

#include "SFML/Window.hpp"


int main(void)
{
	//for us


	NetworkState ns[1] = { 0 };

	const unsigned short SERVER_PORT = 7777;
	const char* SERVER_IP = "172.16.2.57"; //update every time

	ns->m_Peer = RakNet::RakPeerInterface::GetInstance(); //set up peer

	sf::Window window(sf::VideoMode(800, 600), "r/Place");

	while (window.isOpen())
	{

		//Receive Network Messages
		for (RakNet::Packet* packet = ns->m_Peer->Receive(); packet; ns->m_Peer->DeallocatePacket(packet), packet = ns->m_Peer->Receive())
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			NetworkMessage::DecypherPacket(&bsIn, packet->systemAddress, ns->m_ServerMessageQueue);
			//yup, thats it in the input step
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

	}


	RakNet::RakPeerInterface::DestroyInstance(ns->m_Peer);


	return 0;
}
