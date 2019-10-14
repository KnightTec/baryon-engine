#pragma once
#include "Components.h"
#include "Memory.h"


namespace Baryon
{
typedef uint32_t Index;

class Archetype
{
public:
	Archetype(TypeFlag componentTypes, StackAllocator* allocator);
	void addEntity(EntityId entityId);
	void removeEntity(EntityId entityId);
	void moveEntity(Archetype* targetArchetype, TypeFlag targetComponentTypes, EntityId entityId);
	void* getComponent(EntityId entityId, TypeId componentType);
private:
	uint8_t* buffer;
	EntityId* entityIds;
	std::unordered_map<EntityId, Index> entityToComponentIndexMap;
	std::unordered_map<TypeId, size_t> typeOffsets;
	int maxEntitiesPerChunk;
	int numEntities;
};


inline void* Archetype::getComponent(EntityId entityId, TypeId componentType)
{
	auto entityIndex = entityToComponentIndexMap.find(entityId);
	size_t entityOffset = ComponentRegistry::getTypeInfo(componentType).sizeInBytes * entityToComponentIndexMap[entityId];
	return buffer + typeOffsets[componentType] + entityOffset;
}

}
