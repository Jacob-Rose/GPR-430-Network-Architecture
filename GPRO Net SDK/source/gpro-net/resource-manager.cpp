#include "gpro-net/resource-manager.h"

jr::ResourceManager* jr::ResourceManager::sm_Instance = nullptr;

const std::string jr::ResourceManager::PLAYER_TEXTURE_ID = "player_texture";
const std::string jr::ResourceManager::PROJECTILE_TEXTURE_ID = "projectile_texture";
const std::string jr::ResourceManager::ENEMY_TEXTURE_ID = "enemy_texture";
const std::string jr::ResourceManager::PLAYER_ARM_TEXTURE_ID = "player_arm_texture";

jr::ResourceManager::ResourceManager()
{
	init();
}

jr::ResourceManager::~ResourceManager()
{
	cleanup();
}

void jr::ResourceManager::init()
{
	if (!m_IsInit)
	{
		//Player
		m_TextureCache[PLAYER_TEXTURE_ID] = new sf::Texture();
		m_TextureCache[PLAYER_TEXTURE_ID]->loadFromFile("resources/textures/meepop.png");

		//Player Arm
		m_TextureCache[PLAYER_ARM_TEXTURE_ID] = new sf::Texture();
		m_TextureCache[PLAYER_ARM_TEXTURE_ID]->loadFromFile("resources/textures/meepop leg.png");

		//Projectile
		m_TextureCache[PROJECTILE_TEXTURE_ID] = new sf::Texture();
		m_TextureCache[PROJECTILE_TEXTURE_ID]->loadFromFile("resources/textures/kenny/shooting_gallery/HUD/icon_bullet_silver_long.png");

		//Enemy
		m_TextureCache[ENEMY_TEXTURE_ID] = new sf::Texture();
		m_TextureCache[ENEMY_TEXTURE_ID]->loadFromFile("resources/textures/meepop.png");

		m_IsInit = true;
	}
}

void jr::ResourceManager::cleanup()
{
	if (m_IsInit)
	{
		//todo

		for (std::pair<std::string, sf::Texture*> it : m_TextureCache)
		{
			delete it.second;
		}
		m_TextureCache.clear();
		m_IsInit = false;
	}
}

void jr::ResourceManager::initSingleton()
{
	if (sm_Instance == nullptr)
	{
		sm_Instance = new ResourceManager();
	}
	
}

void jr::ResourceManager::cleanupSingleton()
{
	if (sm_Instance != nullptr)
	{
		delete sm_Instance;
		sm_Instance = nullptr;
	}
}
