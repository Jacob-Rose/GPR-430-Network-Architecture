#include "gpro-net/entity.h"

using namespace jr;

void Entity::Update(EntityUpdateInfo info)
{
}

Entity::Entity() : m_OwnerAddress(RakNet::UNASSIGNED_RAKNET_GUID), m_Position(0.0f,0.0f), m_Rotation(0.0f), m_NetID(0)
{

}

Entity::~Entity()
{
}
