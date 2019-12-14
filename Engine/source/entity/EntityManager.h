#pragma once
#include "Archetype.h"
#include "ComponentRegistry.h"
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
	template <typename T, typename... Args>
	void addComponents(EntityId entityId);
	void addComponents(EntityId entityId);
	void addComponents(EntityId entityId, TypeFlag componentTypesFlag);
	template <typename T>
	void removeComponent(EntityId entityId);
	void removeComponent(EntityId entityId, TypeFlag componentTypesFlag);
	template <typename T>
	T* getComponent(EntityId entityId);
	void getArchetypesWithComponents(TypeFlag componentTypesFlag, Archetype** archetypePtrBuffer, int& numArchetypes);
private:
	void changeArchetype(EntityId entityId, TypeFlag newFlag);

	StackAllocator archetypeAllocator;
	std::unordered_map<TypeFlag, Archetype> archetypes;
	std::unordered_map<EntityId, TypeFlag> entityToComponentsMap;
	EntityId nextFreeId;
};


template <typename T, typename... Args>
void EntityManager::addComponents(EntityId entityId)
{
	TypeFlag componentsFlag = ComponentRegistry::getTypeInfo<T>().flag;
	using expander = int[];
	(void)expander {
		0, ((void)(componentsFlag |= ComponentRegistry::getTypeInfo(typeId<Args>()).flag), 0)...
	};
	addComponents(entityId, componentsFlag);
}
inline void EntityManager::addComponents(EntityId entityId, TypeFlag componentTypesFlag)
{
	changeArchetype(entityId, entityToComponentsMap[entityId] | componentTypesFlag);
}
template <typename T>
void EntityManager::removeComponent(EntityId entityId)
{
	removeComponent(entityId, ComponentRegistry::getTypeInfo<T>().flag);
}
inline void EntityManager::removeComponent(EntityId entityId, TypeFlag componentTypesFlag)
{
	changeArchetype(entityId, entityToComponentsMap[entityId] & ~componentTypesFlag);
}
template <typename T>
T* EntityManager::getComponent(EntityId entityId)
{
	auto typeFlagIt = entityToComponentsMap.find(entityId);
	if (typeFlagIt == entityToComponentsMap.end())
	{
		return nullptr;
	}
	auto archetypeIt = archetypes.find(typeFlagIt->second);
	if (archetypeIt == archetypes.end())
	{
		return nullptr;
	}
	Archetype* archetype = &archetypeIt->second;
	return reinterpret_cast<T*>(archetype->getComponent(entityId, typeId<T>()));
}

}
