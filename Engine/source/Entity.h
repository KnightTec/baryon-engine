#pragma once
#include "TransformOld.h"
#include "Mesh.h"

namespace Baryon
{
class Entity
{
public:
	Entity(Mesh& mesh);
	//virtual void update() = 0;
	uint64_t getId() const;

	TransformOld transform;
	Mesh& mesh;
private:
	uint64_t id;
};

inline Entity::Entity(Mesh& mesh) : mesh{mesh}
{
}
inline uint64_t Entity::getId() const
{
	return id;
}

}
