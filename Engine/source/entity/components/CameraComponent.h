#pragma once
#include <DirectXMath.h>

namespace Baryon
{
struct CameraComponent
{
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X3 viewMatrix;

	// used for motion vectors
	DirectX::XMFLOAT4 lastOrientation;
	DirectX::XMFLOAT3 lastPosition;
	DirectX::XMFLOAT3 linearVelocity;
	DirectX::XMFLOAT3 angularVelocity;

	float fov = 70;
	float aspectRatio = 1;
	float nearZ = 0.01;
	float farZ = 1000;

	DirectX::XMFLOAT3 getPosition() const;
};

inline DirectX::XMFLOAT3 CameraComponent::getPosition() const
{
	return DirectX::XMFLOAT3(viewMatrix._41, viewMatrix._42, viewMatrix._43);
}
}
