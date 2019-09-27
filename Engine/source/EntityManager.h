#pragma once
#include "Mesh.h"
#include "Array.h"
#include "Entity.h"

namespace Baryon
{

struct TransformComponent
{
	DirectX::XMFLOAT4 rotationQuaternion;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 size;
};

struct RenderComponent
{
	DirectX::XMFLOAT4X3 worldMatrix;
	//TODO: replace with path
	Mesh* mesh;
};

class EntityManager
{
public:
	EntityManager(size_t maxEntities);
	const Entity& createEntity();
	const void destroyEntity(uint64_t id);
private:
	Array<TransformComponent> transformComponents;
	Array<RenderComponent> renderComponents;

	//TODO: index array


	size_t transformCount;
	size_t renderComponentCount;
};

inline EntityManager::EntityManager(size_t maxEntities) 
	: transformComponents(maxEntities), renderComponents(maxEntities)
{
}
inline const Entity& EntityManager::createEntity()
{
	//TODO:

}
inline const void EntityManager::destroyEntity(uint64_t id)
{
	//TODO:
}



}
