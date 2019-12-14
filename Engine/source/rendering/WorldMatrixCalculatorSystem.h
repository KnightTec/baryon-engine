#pragma once
#include "System.h"
#include "components/Transform.h"
#include "components/Components.h"


namespace Baryon
{
class WorldMatrixCalculatorSystem : public System<Transform, WorldMatrixComponent>
{
	using super = System<Transform, WorldMatrixComponent>;
public:
	WorldMatrixCalculatorSystem(EntityManager* entityManager);
private:
	void update(Transform& transform, WorldMatrixComponent& wmc) override;
};


inline WorldMatrixCalculatorSystem::WorldMatrixCalculatorSystem(EntityManager* entityManager)
	: super(entityManager)
{
}
inline void WorldMatrixCalculatorSystem::update(Transform& transform, WorldMatrixComponent& wmc)
{
	using namespace DirectX;
	XMStoreFloat4x3(&wmc.worldMatrix, XMMatrixScalingFromVector(XMLoadFloat3(&transform.size)) *
		XMMatrixRotationQuaternion(XMLoadFloat4(&transform.orientation)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&transform.position)));
}
}
