#pragma once
#include "Transform.h"
#include "Mesh.h"

namespace Baryon
{
class Entity
{
public:
	Entity(Mesh& mesh);
	//virtual void update() = 0;

	Transform transform;
	Mesh& mesh;
private:
	uint32_t id;
};

inline Entity::Entity(Mesh& mesh) : mesh{mesh}
{
}
}
