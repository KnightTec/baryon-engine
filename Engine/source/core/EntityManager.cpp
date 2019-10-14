#include "EntityManager.h"

using namespace Baryon;

EntityManager::EntityManager()
	: archetypeAllocator(1073741824/*1 Gigabyte*/), entityToComponentsMap(1024), nextFreeId(0)
{
}
EntityId EntityManager::createEntity()
{
	TypeFlag flag = ComponentRegistry::getTypeInfo<Transform>().flag;
	if (archetypes[flag] == nullptr)
	{
		archetypes[flag] = new Archetype(flag, &archetypeAllocator);
	}
	archetypes[flag]->addEntity(nextFreeId);
	entityToComponentsMap[nextFreeId] = flag;
	return nextFreeId++;
}
void EntityManager::destroyEntity(EntityId entityId)
{
	archetypes[entityToComponentsMap[entityId]]->removeEntity(entityId);
	entityToComponentsMap.erase(entityId);
}
void EntityManager::changeArchetype(EntityId entityId, TypeFlag newFlag)
{
	TypeFlag& oldFlag = entityToComponentsMap[entityId];
	if (oldFlag != newFlag) {
		Archetype* newArchetype = archetypes[newFlag];
		if (newArchetype == nullptr)
		{
			newArchetype = new Archetype(newFlag, &archetypeAllocator);
		}
		archetypes[oldFlag]->moveEntity(newArchetype, newFlag, entityId);
		oldFlag = newFlag;
	}
}
