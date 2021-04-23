#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

//SFML
#include <SFML/Graphics/Texture.hpp>

//STD
#include <map>
#include <string>


namespace jr
{
	class ResourceManager;
}



class jr::ResourceManager
{
protected:
	static jr::ResourceManager* sm_Instance;

	ResourceManager();
	~ResourceManager();

	void init();
	void cleanup();

	std::map<std::string, sf::Texture*> m_TextureCache;

	bool m_IsInit = false;
public:
	//Static Info
	static const std::string PROJECTILE_TEXTURE_ID;
	static const std::string PLAYER_TEXTURE_ID;
	static const std::string PLAYER_ARM_TEXTURE_ID;
	static const std::string ENEMY_TEXTURE_ID;

	static void initSingleton();
	static void cleanupSingleton();

	static jr::ResourceManager* getSingleton() { return sm_Instance; }

	//Member Info
	sf::Texture* getTextureFromID(std::string id) { return m_TextureCache[id]; }

};

#endif