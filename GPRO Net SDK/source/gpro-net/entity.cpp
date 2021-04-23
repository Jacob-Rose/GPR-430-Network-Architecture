#include "gpro-net/entity.h"

using namespace jr;

jr::Entity::~Entity()
{
}

void jr::Entity::update(EntityUpdateInfo info)
{
}

jr::Entity::Entity() : m_OwnerAddress(RakNet::UNASSIGNED_RAKNET_GUID), m_Position(0.0f, 0.0f), m_Rotation(0.0f)
{

}