#ifndef _NETWORK_MESSAGES_H
#define _NETWORK_MESSAGES_H

//SFML
#include <SFML/System/Vector2.hpp>

//Std Library
#include <string>
#include <vector>

//RakNet
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/RakString.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTime.h>
#include <RakNet/GetTime.h>
#include <RakNet/PacketPriority.h>


enum class SharedNetworkMessageID
{
	ID_PACKAGED_PACKET = ID_USER_PACKET_ENUM + 1,
	ID_NETWORK_OBJECT_CREATED_MESSAGE,
	ID_NETWORK_OBJECT_DESTROY_MESSAGE,
	ID_NETWORK_OBJECT_UPDATE_MESSAGE,
	ID_NETWORK_OBJECT_AUTHORITY_UPDATED_MESSAGE,
};

enum class NetworkObjectID
{
	NULL_OBJECT_ID,
	PLAYER_OBJECT_ID,
	ENEMY_OBJECT_ID,
	OBJECT_ID_COUNT
};

class TimestampMessage;
class PlayerMoveMessage;
class DisplayNameChangeMessage;
class NetworkMessage;


class NetworkMessage
{
protected:

	const RakNet::MessageID m_MessageID;


	NetworkMessage(RakNet::MessageID id, PacketReliability reliability, PacketPriority priority) : m_MessageID(id), m_Priority(priority), m_Reliablity(reliability) {} //abstract

	
public:
	
	RakNet::RakNetGUID m_Sender;
	PacketReliability m_Reliablity;
	PacketPriority m_Priority;

	RakNet::MessageID getMessageID() { return m_MessageID; }

	virtual bool WritePacketBitstream(RakNet::BitStream* bs) { bs->Write(m_MessageID); return true; }
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) = 0; //all messages should assume the messageid has already been read in and the read index is moved past it


	//We push back each message onto the msgQueue
	//NOTE! All messages will be dynamically allocated
	static void DecypherPacket(RakNet::BitStream* bs, RakNet::RakNetGUID sender, std::vector<NetworkMessage*>& msgQueue);
	static NetworkMessage* DecypherMessage(RakNet::BitStream* bs, RakNet::RakNetGUID sender, RakNet::MessageID id);
	static void CreatePacketHeader(RakNet::BitStream* bs, int msgCount);
};



//Used for messages that are more events that have no information tied to them, connecting, disconnect, all that jazz
class NotificationMessage : public NetworkMessage
{
public:
	NotificationMessage(RakNet::MessageID id) : 
		NetworkMessage((RakNet::MessageID)id, 
			PacketReliability::RELIABLE_ORDERED, 
			PacketPriority::HIGH_PRIORITY) { }

	//these do nothing as the notification message stores the messageID and nothing more
	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override 
	{ 
		NetworkMessage::WritePacketBitstream(bs); 
		return true; 
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override { 
		return true; 
	}
};



class NetworkObjectMessage : public NetworkMessage
{
protected:
	NetworkObjectMessage(SharedNetworkMessageID id, PacketReliability reliability) : 
		NetworkMessage((RakNet::MessageID)id, reliability, PacketPriority::HIGH_PRIORITY), objectId(0) { } //abstract
public:
	short objectId; //directly correlates to object id

	//these do nothing as the notification message stores the messageID and nothing more
	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkMessage::WritePacketBitstream(bs);
		bs->Write(objectId);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		bs->Read(objectId);
		return true;
	}
};

class NetworkObjectAuthorityChangeMessage : public NetworkObjectMessage
{
public:
	RakNet::RakNetGUID newAddress;

	NetworkObjectAuthorityChangeMessage() : 
		NetworkObjectMessage(SharedNetworkMessageID::ID_NETWORK_OBJECT_AUTHORITY_UPDATED_MESSAGE,
			PacketReliability::RELIABLE_ORDERED) {};

	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::WritePacketBitstream(bs);
		bs->Write(newAddress);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::ReadPacketBitstream(bs);
		bs->Read(newAddress);
		return true;
	}
};

class NetworkObjectCreateMessage : public NetworkObjectMessage
{
public:
	NetworkObjectID objectType;

	NetworkObjectCreateMessage() : NetworkObjectMessage(
		SharedNetworkMessageID::ID_NETWORK_OBJECT_CREATED_MESSAGE,
		PacketReliability::RELIABLE_ORDERED), 
		objectType(NetworkObjectID::NULL_OBJECT_ID) {};

	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::WritePacketBitstream(bs);
		bs->Write(objectType);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::ReadPacketBitstream(bs);
		bs->Read(objectType);
		return true;
	}
};

class NetworkObjectDestroyMessage : public NetworkObjectMessage
{
public:
	NetworkObjectDestroyMessage() : NetworkObjectMessage(SharedNetworkMessageID::ID_NETWORK_OBJECT_DESTROY_MESSAGE,
		PacketReliability::RELIABLE_ORDERED) {};

	//no need, we can use NetworkObjectMessage versions with no issue
	//virtual bool WritePacketBitstream(RakNet::BitStream* bs) override;
	//virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override;
};

class NetworkObjectUpdateMessage : public NetworkObjectMessage
{
public:
	float newPos[2];
	float newRot;
	NetworkObjectUpdateMessage() : NetworkObjectMessage(SharedNetworkMessageID::ID_NETWORK_OBJECT_UPDATE_MESSAGE,
		PacketReliability::UNRELIABLE), 
		newPos(), 
		newRot(0) {};

	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::WritePacketBitstream(bs);
		bs->Write(newPos[0]);
		bs->Write(newPos[1]);
		bs->Write(newRot);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::ReadPacketBitstream(bs);
		bs->Read(newPos[0]);
		bs->Read(newPos[1]);
		bs->Read(newRot);
		return true;
	}
};


#endif //_NETWORK_MESSAGES_H