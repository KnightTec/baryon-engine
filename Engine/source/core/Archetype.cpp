#include "Archetype.h"

#include <climits>

using namespace Baryon;

Archetype::Archetype(TypeFlag componentTypes, StackAllocator* allocator)
	: buffer(nullptr), entityIds(nullptr), maxEntitiesPerChunk(INT_MAX), numEntities(0)
{
	size_t bufferSize = 0;
	size_t alignmentOverhead = 0;
	while (componentTypes)
	{
		TypeFlag currentType = componentTypes & -componentTypes;
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
	for (auto& typeOffset : typeOffsets)
	{
		uint8_t* typeAddress = buffer + typeOffset.second;
		size_t typeSize = ComponentRegistry::getTypeInfo(typeOffset.first).sizeInBytes;
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
	Index entityIndex = entityToComponentIndexMap[entityId];
	Index targetEntityIndex = targetArchetype->numEntities - 1;
	uint8_t* targetBuffer = targetArchetype->buffer;
	for (auto& targetTypeOffset : targetArchetype->typeOffsets)
	{
		auto typeOffset = typeOffsets.find(targetTypeOffset.first);
		if (typeOffset != typeOffsets.end())
		{
			size_t typeSize = ComponentRegistry::getTypeInfo(targetTypeOffset.first).sizeInBytes;
			void* src = buffer + typeOffset->second + typeSize * entityIndex;
			void* dst = targetBuffer + targetTypeOffset.second + typeSize * targetEntityIndex;
			memcpy(dst, src, typeSize);
		}
	}
	removeEntity(entityId);
}
void* Archetype::getComponent(EntityId entityId, TypeId componentType)
{
	//TODO: seems to return the wrong memory address

	auto entityIndexIt = entityToComponentIndexMap.find(entityId);
	if (entityIndexIt == entityToComponentIndexMap.end())
	{
		return nullptr;
	}
	size_t entityOffset = ComponentRegistry::getTypeInfo(componentType).sizeInBytes * entityIndexIt->second;
	return buffer + typeOffsets[componentType] + entityOffset;
}