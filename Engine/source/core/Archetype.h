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
	int getEntityCount() const;
	template <typename T>
	T* getComponentArray();
private:
	uint8_t* buffer;
	EntityId* entityIds;
	std::unordered_map<EntityId, Index> entityToComponentIndexMap;
	std::unordered_map<TypeId, size_t> typeOffsets;
	int maxEntitiesPerChunk;
	int numEntities;
};



inline int Archetype::getEntityCount() const
{
	return numEntities;
}
template <typename T>
T* Archetype::getComponentArray()
{
	return reinterpret_cast<T*>(buffer + typeOffsets[typeId<T>()]);
}




}
