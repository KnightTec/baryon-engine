#pragma once
#include "ComponentRegistry.h"
#include "Memory.h"


namespace Baryon
{
typedef uint32_t Index;

/*
 * All entites with the same set of components belong to the same Archetype.
 */
class Archetype
{
public:
	Archetype() = default;
	Archetype(TypeFlag componentTypes, StackAllocator* allocator);
	void addEntity(EntityId entityId, bool constructEntity);
	void removeEntity(EntityId entityId);
	void moveEntity(Archetype* targetArchetype, EntityId entityId);
	void* getComponent(EntityId entityId, TypeId componentType);
	int getEntityCount() const;
	/*
	 * Returns a pointer to the first component of type T
	 */
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
