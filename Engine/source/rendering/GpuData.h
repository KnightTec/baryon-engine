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
	GPUMatrix invViewProj;
	GPUMatrix prevFrameViewProjMat;
	DirectX::XMFLOAT3 cameraPosition;
	float pad0;
	DirectX::XMFLOAT4 cameraLinearVelocity;
	DirectX::XMFLOAT4 cameraAngularVelocity;
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
