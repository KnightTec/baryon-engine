#include "CameraSystem.h"
#include "DirectXMath.h"

using namespace Baryon;

CameraSystem::CameraSystem(EntityManager* entityManager)
	: super(entityManager)
{
}
void CameraSystem::update(Transform& transform, CameraComponent& camera)
{
	using namespace DirectX;
	camera.position = transform.position;
	XMStoreFloat4x4(&camera.projectionMatrix, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(camera.fov), camera.aspectRatio, camera.farZ, camera.nearZ));

	XMMATRIX viewMatrix = XMMatrixTranslationFromVector(-XMLoadFloat3(&transform.position));
	viewMatrix *= XMMatrixTranspose(XMMatrixRotationQuaternion(XMLoadFloat4(&transform.orientation)));
	viewMatrix *= XMMatrixScalingFromVector(XMVectorSet(1, 1, 1, 1) / XMLoadFloat3(&transform.size));
	XMStoreFloat4x3(&camera.viewMatrix, viewMatrix);

	XMStoreFloat3(&camera.linearVelocity, XMLoadFloat3(&transform.position) - XMLoadFloat3(&camera.lastPosition));
	camera.lastPosition = transform.position;

	XMVECTOR orientation = XMLoadFloat4(&transform.orientation);
	XMVECTOR orientationDiff = orientation - XMLoadFloat4(&camera.lastOrientation);
	XMStoreFloat3(&camera.angularVelocity, 2 * orientationDiff * XMQuaternionConjugate(orientation));
	camera.lastOrientation = transform.orientation;

	//TODO: divide velocity by frame time
}