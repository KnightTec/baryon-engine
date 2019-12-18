#include "EntityManager.h"
#include "components/Transform.h"

using namespace Baryon;

EntityManager::EntityManager()
	: archetypeAllocator(1073741824/*1 Gigabyte*/), entityToComponentsMap(1024), nextFreeId(0)
{
}
void EntityManager::destroyEntity(EntityId entityId)
{
	archetypes[entityToComponentsMap[entityId]].removeEntity(entityId);
	entityToComponentsMap.erase(entityId);
}
void EntityManager::getArchetypesWithComponents(TypeFlag componentTypesFlag, Archetype** archetypePtrBuffer, int& numArchetypes)
{
	int archetypeCount = 0;
	for (auto& archetype : archetypes)
	{
		if ((archetype.first & componentTypesFlag) == componentTypesFlag)
		{
			archetypePtrBuffer[archetypeCount++] = &archetype.second;
		}
	}
	if (archetypeCount > numArchetypes)
	{
		numArchetypes = 0;
		archetypePtrBuffer = nullptr;
	} 
	else
	{
		numArchetypes = archetypeCount;
	}
}
void EntityManager::changeArchetype(EntityId entityId, TypeFlag newFlag)
{
	TypeFlag& oldFlag = entityToComponentsMap[entityId];
	if (oldFlag != newFlag) {
		if (archetypes.find(newFlag) == archetypes.end())
		{
			archetypes.emplace(newFlag, Archetype(newFlag, &archetypeAllocator));
		}
		Archetype* newArchetype = &archetypes[newFlag];
		archetypes[oldFlag].moveEntity(newArchetype, entityId);
		oldFlag = newFlag;
	}
}
