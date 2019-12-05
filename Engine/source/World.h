#pragma once
#include "EntityManager.h"

namespace Baryon
{
class World
{
public:
	World(EntityManager* entityManager);
	//void addEntity(Prototype* prototype);
	void load(const char* path);
	void save(const char* path);
private:
	EntityManager* entityManager;
};
}
