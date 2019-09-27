#pragma once
#include "Transform.h"

#include "DirectXMath.h"


namespace Baryon
{

class Camera
{
public:
	Camera(float fov, float aspectRatio, float nearZ, float farZ, DirectX::XMFLOAT3 position = {0, 0, 0});

	void translate(DirectX::XMFLOAT3 vector, SPACE space = SPACE::LOCAL);
	void rotate(float pitch, float yaw, float roll, SPACE space = SPACE::LOCAL);

	DirectX::XMMATRIX getViewMatrix();
	DirectX::XMMATRIX getProjectionMatrix() const;
	DirectX::XMMATRIX getViewProjMatrix();
	DirectX::XMVECTOR getPostion()
	{
		return XMLoadFloat3(&position);
	}

	void setFov(float fov);
	void setAspectRatio(float aspectRatio);
	void setZPlanes(float nearZ, float farZ);
	void setFrustrum(float fov, float aspectRatio, float nearZ, float farZ);
private:
	void updateViewMatrix();

	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X3 viewMatrix;
	DirectX::XMFLOAT4 orientationQuaternion;
	DirectX::XMFLOAT3 position;

	float fov;
	float aspectRatio;
	float nearZ;
	float farZ;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementations 
///////////////////////////////////////////////////////////////////////////////////////////////
inline Camera::Camera(float fov, float aspectRatio, float nearZ, float farZ, DirectX::XMFLOAT3 position)
	: position{position}, fov{fov}, aspectRatio{aspectRatio}, nearZ{nearZ}, farZ{farZ}
{
	XMStoreFloat4(&orientationQuaternion, DirectX::XMQuaternionIdentity());
	setFrustrum(fov, aspectRatio, nearZ, farZ);
}
inline DirectX::XMMATRIX Camera::getViewMatrix()
{
	updateViewMatrix();
	return XMLoadFloat4x3(&viewMatrix);
}
inline DirectX::XMMATRIX Camera::getProjectionMatrix() const
{
	return XMLoadFloat4x4(&projectionMatrix);
}
inline DirectX::XMMATRIX Camera::getViewProjMatrix()
{
	return getViewMatrix() * getProjectionMatrix();
}
inline void Camera::setFrustrum(float fov, float aspectRatio, float nearZ, float farZ)
{
	// swap near and far z for inverted floating point depth
	XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(fov), aspectRatio, farZ, nearZ));
}
inline void Camera::setFov(float fov)
{
	this->fov = fov;
	setFrustrum(fov, aspectRatio, nearZ, farZ);
}
inline void Camera::setAspectRatio(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
	setFrustrum(fov, aspectRatio, nearZ, farZ);
}
inline void Camera::setZPlanes(float nearZ, float farZ)
{
	this->nearZ = nearZ;
	this->farZ = farZ;
	setFrustrum(fov, aspectRatio, nearZ, farZ);
}
}
