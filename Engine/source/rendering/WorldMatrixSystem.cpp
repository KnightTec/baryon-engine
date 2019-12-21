#include "WorldMatrixSystem.h"

using namespace Baryon;

WorldMatrixSystem::WorldMatrixSystem(EntityManager* entityManager)
	: super(entityManager)
{
}
void WorldMatrixSystem::update(Transform& transform, WorldMatrixComponent& wmc)
{
	using namespace DirectX;
	XMStoreFloat4x3(&wmc.worldMatrix, XMMatrixScalingFromVector(XMLoadFloat3(&transform.size)) *
		XMMatrixRotationQuaternion(XMLoadFloat4(&transform.orientation)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&transform.position)));
}