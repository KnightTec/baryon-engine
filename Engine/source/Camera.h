#pragma once
#include "DirectXMath.h"

namespace Baryon
{
class Camera
{
public:
	Camera(float fov, float aspectRatio, float nearZ, float farZ, DirectX::XMFLOAT3 position = { 0, 0, 0 });

	void translate(DirectX::XMFLOAT3 vector);
	void rotate();

	DirectX::XMMATRIX getViewMatrix();
	DirectX::XMMATRIX getProjectionMatrix() const;
	DirectX::XMMATRIX getViewProjMatrix();

	void setFov(float fov);
	void setAspectRatio(float aspectRatio);
	void setZPlanes(float nearZ, float farZ);
	void setFrustrum(float fov, float aspectRatio, float nearZ, float farZ);
private:
	void updateViewMatrix();

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;

	float fov;
	float aspectRatio;
	float nearZ;
	float farZ;
};

inline Camera::Camera(float fov, float aspectRatio, float nearZ, float farZ, DirectX::XMFLOAT3 position)
	: position{position}, fov{fov}, aspectRatio{aspectRatio}, nearZ{nearZ}, farZ{farZ}
{
	setFrustrum(fov, aspectRatio, nearZ, farZ);
}
inline void Camera::translate(DirectX::XMFLOAT3 vector)
{
	XMStoreFloat3(
		&position, XMVector3Transform(XMLoadFloat3(&position),
		                              DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&vector)))
	);
}
inline DirectX::XMMATRIX Camera::getViewMatrix()
{
	updateViewMatrix();
	return XMLoadFloat4x4(&viewMatrix);
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
	XMStoreFloat4x4(
		&projectionMatrix,
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, nearZ, farZ));
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
