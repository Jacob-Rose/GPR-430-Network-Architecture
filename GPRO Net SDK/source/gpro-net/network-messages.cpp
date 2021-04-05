#include "gpro-net/network-messages.h"

void NetworkMessage::DecypherPacket(RakNet::BitStream* bs, RakNet::RakNetGUID sender, std::vector<NetworkMessage*>& msgQueue)
{
	
	RakNet::MessageID id;
	bs->Read(id); //now for each message constructor, THE MESSAGE IS TRIMMED OFF THE FRONT, SO THE READ DOES NOT NEED TO ADDRESS THIS MESSAGE ID WE ALREADY KNOW
	//possibly read in id count
	
	if (id == (RakNet::MessageID)SharedNetworkMessageID::ID_PACKAGED_PACKET)
	{
		int idCount = 0;
		bs->Read(idCount);

		//Now read message per
		bs->Read(id);
		for (int i = 0; i < idCount; i++)
		{
			msgQueue.push_back(DecypherMessage(bs, sender, id));
		}
	}
	else
	{
		msgQueue.push_back(DecypherMessage(bs, sender, id));
	}
	
}

NetworkMessage* NetworkMessage::DecypherMessage(RakNet::BitStream* bs, RakNet::RakNetGUID sender, RakNet::MessageID id)
{
	NetworkMessage* msg;
	switch (id) {
	default:
		msg = new NotificationMessage(id);
		break;
	case (RakNet::MessageID)SharedNetworkMessageID::ID_NETWORK_OBJECT_CREATED_MESSAGE:
		msg = new NetworkObjectCreateMessage();
		break;
	case (RakNet::MessageID)SharedNetworkMessageID::ID_NETWORK_OBJECT_DESTROY_MESSAGE:
		msg = new NetworkObjectDestroyMessage();
		break;
	case (RakNet::MessageID)SharedNetworkMessageID::ID_NETWORK_OBJECT_UPDATE_MESSAGE:
		msg = new NetworkObjectUpdateMessage();
		break;
	}
	msg->m_Sender = sender;
	msg->ReadPacketBitstream(bs);
	return msg;
}

void NetworkMessage::CreatePacketHeader(RakNet::BitStream* bs, int msgCount)
{
	bs->Write((RakNet::MessageID)SharedNetworkMessageID::ID_PACKAGED_PACKET);
	bs->Write(msgCount);
}