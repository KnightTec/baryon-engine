#include "Camera.h"

using namespace Baryon;
using namespace DirectX;


void Camera::translate(DirectX::XMFLOAT3 vector, SPACE space)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&orientationQuaternion));

	XMVECTOR translationVec = XMLoadFloat3(&vector);

	if (space == SPACE::LOCAL)
	{
		translationVec = XMVector3Transform(translationVec, rotation);
	}
	XMStoreFloat3(&position, XMVectorAdd(pos, translationVec));
}

void Camera::rotate(float pitch, float yaw, float roll, SPACE space)
{
	XMVECTOR newRotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMVECTOR rotation = XMLoadFloat4(&orientationQuaternion);

	rotation = space == SPACE::LOCAL ? XMQuaternionMultiply(newRotation, rotation) : XMQuaternionMultiply(rotation, newRotation);

	XMStoreFloat4(&orientationQuaternion, XMQuaternionNormalize(rotation));
}

void Camera::updateViewMatrix()
{
	XMMATRIX viewMat = XMMatrixTranslationFromVector(-XMLoadFloat3(&position));
	viewMat *= XMMatrixTranspose(XMMatrixRotationQuaternion(XMLoadFloat4(&orientationQuaternion)));
	XMStoreFloat4x4(&viewMatrix, viewMat);
}
