#include "Archetype.h"

#include <climits>

using namespace Baryon;

Archetype::Archetype(TypeFlag componentTypes, StackAllocator* allocator)
	: buffer(nullptr), entityIds(nullptr), maxEntitiesPerChunk(INT_MAX), numEntities(0)
{
	size_t bufferSize = 0;
	size_t alignmentOverhead = 0;
	TypeFlag currentType;
	while (componentTypes)
	{
		currentType = componentTypes & -componentTypes;
		componentTypes &= ~currentType;
		auto& typeInfo = ComponentRegistry::getTypeInfo(currentType);
		if (typeInfo.countPerChunk < maxEntitiesPerChunk)
		{
			maxEntitiesPerChunk = typeInfo.countPerChunk;
		}
		bufferSize += typeInfo.sizeInBytes;
		alignmentOverhead += typeInfo.alignment;
		typeOffsets[typeInfo.typeId] = 0;
	}

	bufferSize = bufferSize * maxEntitiesPerChunk + alignmentOverhead;
	buffer = static_cast<uint8_t*>(allocator->allocate(bufferSize));
	entityIds = static_cast<EntityId*>(allocator->allocate(maxEntitiesPerChunk * sizeof EntityId));

	size_t nextOffset = 0;
	for (auto& it : typeOffsets)
	{
		it.second = nextOffset;

		auto typeAddress = reinterpret_cast<size_t>(buffer) + it.second;
		auto& typeInfo = ComponentRegistry::getTypeInfo(it.first);

		// align memory address
		size_t mask = typeInfo.alignment - 1;
		it.second += typeInfo.alignment - (mask & typeAddress);

		nextOffset = it.second + typeInfo.sizeInBytes;
	}
}
void Archetype::addEntity(EntityId entityId)
{
	for (auto& type : typeOffsets)
	{
		ZeroMemory(buffer + type.second + numEntities, ComponentRegistry::getTypeInfo(type.first).sizeInBytes);
	}
	entityToComponentIndexMap[entityId] = numEntities;
	entityIds[numEntities++] = entityId;
}
void Archetype::removeEntity(EntityId entityId)
{
	Index lastIndex = --numEntities;
	Index removedEntityIndex = entityToComponentIndexMap[entityId];
	for (auto& component : typeOffsets)
	{
		uint8_t* typeAddress = buffer + typeOffsets[component.first];
		size_t typeSize = ComponentRegistry::getTypeInfo(component.first).sizeInBytes;
		memcpy(typeAddress + removedEntityIndex * typeSize, typeAddress + lastIndex * typeSize, typeSize);
	}
	EntityId movedEntityId = entityIds[lastIndex];
	entityIds[removedEntityIndex] = movedEntityId;
	entityToComponentIndexMap[movedEntityId] = removedEntityIndex;
	entityToComponentIndexMap.erase(entityId);
}
void Archetype::moveEntity(Archetype* targetArchetype, TypeFlag targetComponentTypes, EntityId entityId)
{
	targetArchetype->addEntity(entityId);
	for (auto& componentType : typeOffsets)
	{
		//TODO: copy data
	}
	removeEntity(entityId);
}