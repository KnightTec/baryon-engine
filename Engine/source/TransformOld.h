#pragma once
#include <DirectXMath.h>

namespace Baryon
{
enum class SPACE
{
	LOCAL,
	GLOBAL
};

class TransformOld
{
public:
	TransformOld();
	void translate(float x, float y, float z, SPACE space = SPACE::LOCAL);
	void rotate(float pitch, float yaw, float roll, SPACE space = SPACE::LOCAL);
	void scale(float x, float y, float z, SPACE space = SPACE::LOCAL);

	DirectX::XMMATRIX getWorldMatrix() const;
private:
	DirectX::XMFLOAT4 rotationQuaternion;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 size;
};


inline TransformOld::TransformOld() : rotationQuaternion(0, 0, 0, 1), position(0, 0, 0), size(1, 1, 1)
{
}
inline void TransformOld::translate(float x, float y, float z, SPACE space)
{
	using namespace DirectX;
	XMVECTOR translationVec = XMVectorSet(x, y, z, 1);
	if (space == SPACE::LOCAL)
	{
		translationVec = XMVector3Rotate(translationVec, XMLoadFloat4(&rotationQuaternion));
	}
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), translationVec));
}
inline void TransformOld::rotate(float pitch, float yaw, float roll, SPACE space)
{
	using namespace DirectX;
	XMVECTOR newRotation = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMVECTOR rotation = XMLoadFloat4(&rotationQuaternion);

	rotation = space == SPACE::LOCAL ? XMQuaternionMultiply(newRotation, rotation) : XMQuaternionMultiply(rotation, newRotation);

	XMStoreFloat4(&rotationQuaternion, XMQuaternionNormalize(rotation));
}
inline void TransformOld::scale(float x, float y, float z, SPACE space)
{
	//TODO: global scale
	using namespace DirectX;
	size.x *= x;
	size.y *= y;
	size.z *= z;
}

inline DirectX::XMMATRIX TransformOld::getWorldMatrix() const
{
	using namespace DirectX;
	return XMMatrixScalingFromVector(XMLoadFloat3(&size)) * XMMatrixRotationQuaternion(XMLoadFloat4(&rotationQuaternion)) * XMMatrixTranslationFromVector(XMLoadFloat3(&position));
}
}
