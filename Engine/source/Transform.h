#pragma once
#include <DirectXMath.h>

namespace Baryon
{
enum class SPACE
{
	LOCAL,
	GLOBAL
};

class Transform
{
public:
	Transform();
	void translate(float x, float y, float z, SPACE space = SPACE::LOCAL);
	void rotate(float pitch, float yaw, float roll, SPACE space = SPACE::LOCAL);
	void scale(float x, float y, float z, SPACE space = SPACE::LOCAL);
	DirectX::XMMATRIX getMatrix() const;

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 rotationQuaternion;
	DirectX::XMFLOAT3 size;
};


inline Transform::Transform() : position(0, 0, 0), rotationQuaternion(0, 0, 0, 1), size(1, 1, 1)
{
}
inline void Transform::translate(float x, float y, float z, SPACE space)
{
	using namespace DirectX;
	XMVECTOR translationVec = XMVectorSet(x, y, z, 1);
	if (space == SPACE::LOCAL)
	{
		translationVec = XMVector3Rotate(translationVec, XMLoadFloat4(&rotationQuaternion));
	}
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), translationVec));
}
inline void Transform::rotate(float pitch, float yaw, float roll, SPACE space)
{
	using namespace DirectX;
	XMVECTOR newRotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMVECTOR rotation = XMLoadFloat4(&rotationQuaternion);

	rotation = space == SPACE::LOCAL ? XMQuaternionMultiply(newRotation, rotation) : XMQuaternionMultiply(rotation, newRotation);

	XMStoreFloat4(&rotationQuaternion, XMQuaternionNormalize(rotation));
}
inline void Transform::scale(float x, float y, float z, SPACE space)
{
	//TODO: local scale
	using namespace DirectX;
	size.x *= x;
	size.y *= y;
	size.z *= z;
}

inline DirectX::XMMATRIX Transform::getMatrix() const
{
	using namespace DirectX;
	return XMMatrixScalingFromVector(XMLoadFloat3(&size)) * XMMatrixRotationQuaternion(XMLoadFloat4(&rotationQuaternion)) * XMMatrixTranslationFromVector(XMLoadFloat3(&position));
}
}
