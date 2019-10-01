#pragma once
#include "Archetype.h"
#include "Components.h"
#include "Memory.h"

#include <unordered_map>


namespace Baryon
{
class EntityManager
{
public:
	EntityManager();
	EntityId createEntity();
	void destroyEntity(EntityId entityId);
	template <typename T>
	void addComponent(EntityId entityId);
	void addComponent(EntityId entityId, TypeId componentType);
	template <typename T>
	void removeComponent(EntityId entityId);
	void removeComponent(EntityId entityId, TypeId componentType);
	template <typename T>
	T* getComponent(EntityId entityId);
private:
	void changeArchetype(EntityId entityId, TypeFlag newFlag);

	StackAllocator archetypeAllocator;
	std::unordered_map<TypeFlag, Archetype*> archetypes;
	std::unordered_map<EntityId, TypeFlag> entityToComponentsMap;
	EntityId nextFreeId;
};


template <typename T>
void EntityManager::addComponent(EntityId entityId)
{
	changeArchetype(entityId, entityToComponentsMap[entityId] | ComponentRegistry::getTypeInfo<T>().flag);
}
template <typename T>
void EntityManager::removeComponent(EntityId entityId)
{
	changeArchetype(entityId, entityToComponentsMap[entityId] & ~ComponentRegistry::getTypeInfo<T>().flag);
}



template <typename... HandledComponents>
class System
{
	// getComponentArray
};
}
