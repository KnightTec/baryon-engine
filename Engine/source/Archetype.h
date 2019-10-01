#pragma once
#include "Components.h"
#include "Memory.h"

namespace Baryon
{
//TODO: create archetype based on flags

class Archetype
{
public:
	Archetype(TypeFlag componentTypes, StackAllocator* allocator);
	void addEntity(EntityId entityId);
	void removeEntity(EntityId entityId);
	void moveEntity(Archetype* other, TypeFlag otherTypes, EntityId entityId);
	template <typename T>
	T* getComponentPtr()
	{
		return static_cast<T*>(typeAddress[typeId<T>()]);
	}
private:
	void* buffer;
	std::unordered_map<EntityId, int> entityToComponentIndexMap;
	std::unordered_map<TypeId, void*> typeAddress;
	int numEntities;
	int maxEntities;
};
}
