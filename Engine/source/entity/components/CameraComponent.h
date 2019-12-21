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

	DirectX::XMFLOAT3 position;

	float fov = 70;
	float aspectRatio = 1;
	float nearZ = 0.01;
	float farZ = 1000;

	DirectX::XMMATRIX getViewProjectionXM() const;
};

inline DirectX::XMMATRIX CameraComponent::getViewProjectionXM() const
{
	using namespace DirectX;
	return XMLoadFloat4x3(&viewMatrix) * XMLoadFloat4x4(&projectionMatrix);
}
}
