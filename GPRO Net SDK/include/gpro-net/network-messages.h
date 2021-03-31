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


enum SharedNetworkMessageID
{
	ID_PACKAGED_PACKET = ID_USER_PACKET_ENUM + 1,
	ID_NETWORK_OBJECT_CREATED_MESSAGE,
	ID_NETWORK_OBJECT_DESTROY_MESSAGE,
	ID_NETWORK_ENTITY_UPDATE_MESSAGE,
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
	NetworkMessage(RakNet::MessageID id) : m_MessageID(id) {}
public:
	const RakNet::MessageID m_MessageID;
	RakNet::RakNetGUID m_Sender;



public:
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
	NotificationMessage(RakNet::MessageID id) : NetworkMessage(id) { }

	//these do nothing as the notification message stores the messageID and nothing more
	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override { NetworkMessage::WritePacketBitstream(bs); return true; }
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override { return true; }
};



class NetworkObjectMessage : public NetworkMessage
{
public:
	short objectId; //directly correlates to object id

	NetworkObjectMessage(SharedNetworkMessageID id) : NetworkMessage(id), objectId(0) { }

	//these do nothing as the notification message stores the messageID and nothing more
	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override;
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override;
};

class NetworkObjectAuthorityChangeMessage : public NetworkObjectMessage
{
public:
	RakNet::RakNetGUID newAddress;

	NetworkObjectAuthorityChangeMessage() : NetworkObjectMessage(ID_NETWORK_OBJECT_AUTHORITY_UPDATED_MESSAGE) {};
};

class NetworkObjectCreateMessage : public NetworkObjectMessage
{
public:
	NetworkObjectID objectType;

	NetworkObjectCreateMessage() : NetworkObjectMessage(ID_NETWORK_OBJECT_CREATED_MESSAGE), objectType(NetworkObjectID::NULL_OBJECT_ID) {};
};

class NetworkObjectDestroyMessage : public NetworkObjectMessage
{
public:
	NetworkObjectDestroyMessage() : NetworkObjectMessage(ID_NETWORK_OBJECT_DESTROY_MESSAGE) {};
};

class NetworkObjectUpdateMessage : public NetworkObjectMessage
{
	float newPos[2];
	float newRot;
public:
	NetworkObjectUpdateMessage() : NetworkObjectMessage(ID_NETWORK_ENTITY_UPDATE_MESSAGE), newPos(), newRot(0) {};
};


#endif //_NETWORK_MESSAGES_H