#pragma once
#include <DirectXMath.h>

namespace Baryon
{
class GPUMatrix
{
public:
	GPUMatrix() = default;
	GPUMatrix(DirectX::XMMATRIX& xmMatrix);
	GPUMatrix(DirectX::XMMATRIX&& xmMatrix);
	GPUMatrix& operator=(DirectX::XMMATRIX& xmMatrix);
	GPUMatrix& operator=(DirectX::XMMATRIX&& xmMatrix);
private:
	DirectX::XMFLOAT4X4 matrix;
};

struct PER_OBJECT_DATA
{
	GPUMatrix worldViewProjMat;
	GPUMatrix worldNormalsMat;
};
struct PER_CAMERA_DATA
{
	DirectX::XMFLOAT2 screenParams;
	float pad[2];
	GPUMatrix viewProjection;
	GPUMatrix invViewProj;
	GPUMatrix prevFrameViewProjMat;
	DirectX::XMFLOAT3 cameraPosition;
	float pad1;
	DirectX::XMFLOAT3 cameraLinearVelocity;
	float pad2;
	DirectX::XMFLOAT3 cameraAngularVelocity;
	float pad3;
};


inline GPUMatrix::GPUMatrix(DirectX::XMMATRIX& xmMatrix)
{
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixTranspose(xmMatrix));
}
inline GPUMatrix::GPUMatrix(DirectX::XMMATRIX&& xmMatrix)
{
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixTranspose(xmMatrix));
}
inline GPUMatrix& GPUMatrix::operator=(DirectX::XMMATRIX& xmMatrix)
{
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixTranspose(xmMatrix));
	return *this;
}
inline GPUMatrix& GPUMatrix::operator=(DirectX::XMMATRIX&& xmMatrix)
{
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixTranspose(xmMatrix));
	return *this;
}
}
