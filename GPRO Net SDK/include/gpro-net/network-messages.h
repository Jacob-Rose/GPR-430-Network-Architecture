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

#include <gpro-net/netid.h>


#pragma warning( disable : 26812) //Warning for EnumShouldBeEnumClass | We cannot fix most of these issues as they are rooted in RakNet and I do not want to modify


enum class SharedNetworkMessageID
{
	ID_PACKAGED_PACKET = ID_USER_PACKET_ENUM + 1,
	ID_NETWORK_OBJECT_CREATED_MESSAGE,
	ID_NETWORK_OBJECT_CREATE_REQUEST_MESSAGE,
	ID_NETWORK_OBJECT_DESTROY_MESSAGE,
	ID_NETWORK_OBJECT_UPDATE_MESSAGE,
	ID_NETWORK_OBJECT_HARD_UPDATE_MESSAGE,
	ID_NETWORK_OBJECT_AUTHORITY_UPDATED_MESSAGE,
	ID_NETWORK_PROJECTILE_HIT_MESSAGE,
	ID_NETWORK_PLAYER_HEALTH_UPDATE_MESSAGE,
	ID_PLAYER_PAINT_SHOT_MESSAGE
};

enum class NetworkObjectID
{
	NULL_OBJECT_ID,
	PLAYER_OBJECT_ID,
	BULLET_OBJECT_ID,
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


	NetworkMessage(RakNet::MessageID id, PacketReliability reliability, PacketPriority priority) : 
		m_MessageID(id), m_Priority(priority), m_Reliablity(reliability), m_Target(RakNet::UNASSIGNED_RAKNET_GUID) {} //abstract

	
public:
	
	RakNet::RakNetGUID m_Sender;
	PacketReliability m_Reliablity;
	PacketPriority m_Priority;
	RakNet::RakNetGUID m_Target;

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

class NetworkObjectRequestCreateMessage : public NetworkMessage
{
public:
	NetworkObjectID objectType;
	float objectPos[2];
	float objectRot;

	NetworkObjectRequestCreateMessage() : 
		NetworkMessage((RakNet::MessageID)SharedNetworkMessageID::ID_NETWORK_OBJECT_CREATE_REQUEST_MESSAGE,
		PacketReliability::RELIABLE_ORDERED,
		PacketPriority::HIGH_PRIORITY) { }

	//these do nothing as the notification message stores the messageID and nothing more
	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkMessage::WritePacketBitstream(bs);
		bs->Write(objectType);
		bs->Write(objectPos[0]);
		bs->Write(objectPos[1]);
		bs->Write(objectRot);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override {
		bs->Read(objectType);
		bs->Read(objectPos[0]);
		bs->Read(objectPos[1]);
		bs->Read(objectRot);
		return true;
	}
};

class NetworkProjectileHitMessage : public NetworkMessage
{
public:

	NetworkProjectileHitMessage() :
		NetworkMessage((RakNet::MessageID)SharedNetworkMessageID::ID_NETWORK_PROJECTILE_HIT_MESSAGE, PacketReliability::RELIABLE, PacketPriority::HIGH_PRIORITY) { } //abstract

	jr::NetID m_ProjectileNetID;
	jr::NetID m_HitObjectNetID; 

	//these do nothing as the notification message stores the messageID and nothing more
	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkMessage::WritePacketBitstream(bs);
		bs->Write(m_ProjectileNetID.id);
		bs->Write(m_ProjectileNetID.layer);
		bs->Write(m_HitObjectNetID.id);
		bs->Write(m_HitObjectNetID.layer);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		bs->Read(m_ProjectileNetID.id);
		bs->Read(m_ProjectileNetID.layer);
		bs->Read(m_HitObjectNetID.id);
		bs->Read(m_HitObjectNetID.layer);
		return true;
	}
};

/*
class PlayerShotPaintMessage : public NetworkMessage
{
public:
	int pixelPos[2];
	int pixelColorIndex;

	PlayerShotPaintMessage() :
		NetworkMessage((RakNet::MessageID)SharedNetworkMessageID::ID_PLAYER_PAINT_SHOT_MESSAGE,
			PacketReliability::RELIABLE_ORDERED,
			PacketPriority::HIGH_PRIORITY) { }
	
	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkMessage::WritePacketBitstream(bs);
		bs->Write(pixelPos[0]);
		bs->Write(pixelPos[1]);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		bs->Read(pixelPos[0]);
		bs->Read(pixelPos[1]);
		return true;
	}
};

class PaintPixelMessage : public NetworkMessage
{
	int pixelPos[2];


};
*/

// |||||||||| NETWORK OBJECT MESSAGES |||||||||||||||||||

class NetworkObjectMessage : public NetworkMessage
{
protected:
	NetworkObjectMessage(SharedNetworkMessageID id, PacketReliability reliability, PacketPriority priority = PacketPriority::MEDIUM_PRIORITY) : 
		NetworkMessage((RakNet::MessageID)id, reliability, priority) { } //abstract
public:
	jr::NetID m_NetID; //directly correlates to object id

	//these do nothing as the notification message stores the messageID and nothing more
	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkMessage::WritePacketBitstream(bs);
		bs->Write(m_NetID.id);
		bs->Write(m_NetID.layer);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		bs->Read(m_NetID.id);
		bs->Read(m_NetID.layer);
		return true;
	}
};

class NetworkObjectAuthorityChangeMessage : public NetworkObjectMessage
{
public:
	RakNet::RakNetGUID newAddress;

	NetworkObjectAuthorityChangeMessage() : 
		NetworkObjectMessage(SharedNetworkMessageID::ID_NETWORK_OBJECT_AUTHORITY_UPDATED_MESSAGE,
			PacketReliability::RELIABLE_ORDERED) {}

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
		objectType(NetworkObjectID::NULL_OBJECT_ID) {}

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
		PacketReliability::RELIABLE_ORDERED) {}

	//no need, we can use NetworkObjectMessage versions with no issue
	//virtual bool WritePacketBitstream(RakNet::BitStream* bs) override;
	//virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override;
};

class NetworkObjectUpdateMessage : public NetworkObjectMessage
{
public:
	float newPos[2];
	float newVelocity[2];
	float newRot;
	bool hard; //do we dead-reckon?
	NetworkObjectUpdateMessage() : NetworkObjectMessage(SharedNetworkMessageID::ID_NETWORK_OBJECT_UPDATE_MESSAGE,
		PacketReliability::UNRELIABLE), 
		newPos(), 
		newVelocity(),
		newRot(0),
		hard(false)
	{}

	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::WritePacketBitstream(bs);
		bs->Write(newPos[0]);
		bs->Write(newPos[1]);
		bs->Write(newVelocity[0]);
		bs->Write(newVelocity[1]);
		bs->Write(newRot);
		bs->Write(hard);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::ReadPacketBitstream(bs);
		bs->Read(newPos[0]);
		bs->Read(newPos[1]);
		bs->Read(newVelocity[0]);
		bs->Read(newVelocity[1]);
		bs->Read(newRot);
		bs->Read(hard);
		return true;
	}
};

class NetworkPlayerKilledPlayerEventMessage : public NetworkMessage
{
public:
	jr::NetID m_KillerID;
	jr::NetID m_DeadDudesID;
	int m_KillerNewScore;
	NetworkPlayerKilledPlayerEventMessage() : NetworkMessage((RakNet::MessageID)SharedNetworkMessageID::ID_NETWORK_OBJECT_UPDATE_MESSAGE,
		PacketReliability::RELIABLE, PacketPriority::HIGH_PRIORITY)
	{}

	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkMessage::WritePacketBitstream(bs);
		bs->Write(m_KillerID.id);
		bs->Write(m_KillerID.layer);
		bs->Write(m_DeadDudesID.id);
		bs->Write(m_DeadDudesID.layer);
		bs->Write(m_KillerNewScore);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		bs->Read(m_KillerID.id);
		bs->Read(m_KillerID.layer);
		bs->Read(m_DeadDudesID.id);
		bs->Read(m_DeadDudesID.layer);
		bs->Read(m_KillerNewScore);
		return true;
	}
};

class NetworkPlayerHealthUpdateMessage : public NetworkObjectMessage
{
public:
	float newHealth;
	NetworkPlayerHealthUpdateMessage() : NetworkObjectMessage(SharedNetworkMessageID::ID_NETWORK_PLAYER_HEALTH_UPDATE_MESSAGE,
		PacketReliability::RELIABLE), newHealth(0.0f)
	{}

	virtual bool WritePacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::WritePacketBitstream(bs);
		bs->Write(newHealth);
		return true;
	}
	virtual bool ReadPacketBitstream(RakNet::BitStream* bs) override
	{
		NetworkObjectMessage::ReadPacketBitstream(bs);
		bs->Read(newHealth);
		return true;
	}
};

#endif //_NETWORK_MESSAGES_H