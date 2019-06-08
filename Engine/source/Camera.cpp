#include "Camera.h"

using namespace Baryon;
using namespace DirectX;

void Camera::updateViewMatrix()
{
	//TODO: rotation
	XMMATRIX viewMat = XMMatrixRotationQuaternion(XMLoadFloat4(&orientationQuaternion));
	viewMat *= XMMatrixTranslationFromVector(-XMLoadFloat3(&position));
	XMStoreFloat4x4(&viewMatrix, viewMat);
}
