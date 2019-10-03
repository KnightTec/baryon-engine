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
	template <typename T>
	T* getComponentPtr()
	{
		return static_cast<T*>(typeOffsets[typeId<T>()]);
	}
private:
	uint8_t* buffer;
	EntityId* entityIds;
	std::unordered_map<EntityId, Index> entityToComponentIndexMap;
	std::unordered_map<TypeId, size_t> typeOffsets;
	int maxEntitiesPerChunk;
	int numEntities;
};
}
