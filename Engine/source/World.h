#pragma once
#include "EntityManager.h"
#include "components/Components.h"
#include "components/CameraComponent.h"
#include "Entity.h"

namespace Baryon
{
class World
{
public:
	World(EntityManager* entityManager);

	Entity* addEmpty();
	Entity* addMesh();
	Entity* addCamera();
private:
	EntityManager* entityManager;
	PoolAllocator<Entity, 1024> entityPool;
	//Terrain
};


inline World::World(EntityManager* entityManager)
	: entityManager(entityManager)
{
}
inline Entity* World::addEmpty()
{
	return new(entityPool.allocate()) Entity(entityManager->createEntity(), entityManager);
}
inline Entity* World::addMesh()
{
	return new(entityPool.allocate())
		Entity(entityManager->createEntity<WorldMatrixComponent, MeshComponent>(), entityManager);
}
inline Entity* World::addCamera()
{
	return new(entityPool.allocate()) Entity(entityManager->createEntity<CameraComponent>(), entityManager);
}
}
