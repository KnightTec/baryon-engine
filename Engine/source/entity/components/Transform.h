#pragma once
#include "DirectXMath.h"

namespace Baryon
{
enum class SPACE
{
	LOCAL,
	GLOBAL
};

struct Transform
{
	Transform();
	void translate(float x, float y, float z, SPACE space = SPACE::LOCAL);
	void rotate(float pitch, float yaw, float roll, SPACE space = SPACE::LOCAL);
	void scale(float x, float y, float z, SPACE space = SPACE::LOCAL);
	DirectX::XMMATRIX computeWorldMatrix() const;
private:
	DirectX::XMFLOAT4 orientation;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 size;
};



inline Transform::Transform() : orientation(0, 0, 0, 1), position(0, 0, 0), size(1, 1, 1)
{
}
inline void Transform::translate(float x, float y, float z, SPACE space)
{
	using namespace DirectX;
	XMVECTOR translationVec = XMVectorSet(x, y, z, 1);
	if (space == SPACE::LOCAL)
	{
		translationVec = XMVector3Rotate(translationVec, XMLoadFloat4(&orientation));
	}
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), translationVec));
}
inline void Transform::rotate(float pitch, float yaw, float roll, SPACE space)
{
	using namespace DirectX;
	XMVECTOR newRotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMVECTOR rotation = XMLoadFloat4(&orientation);

	rotation = space == SPACE::LOCAL ? XMQuaternionMultiply(newRotation, rotation) : XMQuaternionMultiply(rotation, newRotation);

	XMStoreFloat4(&orientation, XMQuaternionNormalize(rotation));
}
inline void Transform::scale(float x, float y, float z, SPACE space)
{
	//TODO: global scale
	using namespace DirectX;
	size.x *= x;
	size.y *= y;
	size.z *= z;
}
inline DirectX::XMMATRIX Transform::computeWorldMatrix() const
{
	using namespace DirectX;
	return XMMatrixScalingFromVector(XMLoadFloat3(&size)) *
		XMMatrixRotationQuaternion(XMLoadFloat4(&orientation)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&position));
}
}
