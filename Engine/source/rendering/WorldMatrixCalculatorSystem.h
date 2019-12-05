#pragma once
#include "System.h"
#include "components/Transform.h"


namespace Baryon
{
class WorldMatrixCalculatorSystem : public System<Transform, StaticMesh>
{
	using super = System<Transform, StaticMesh>;
public:
	WorldMatrixCalculatorSystem(EntityManager* entityManager);
private:
	void update(Transform& transform, StaticMesh& staticMesh) override;
};


inline WorldMatrixCalculatorSystem::WorldMatrixCalculatorSystem(EntityManager* entityManager)
	: super(entityManager)
{
}
inline void WorldMatrixCalculatorSystem::update(Transform& transform, StaticMesh& staticMesh)
{
	XMStoreFloat4x3(&staticMesh.worldMatrix, transform.computeWorldMatrix());
}
}
