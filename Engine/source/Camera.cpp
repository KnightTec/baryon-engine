#include "Camera.h"

using namespace Baryon;
using namespace DirectX;

void Camera::updateViewMatrix()
{
	//TODO: rotation
	XMVECTOR posInverse = -XMLoadFloat3(&position);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranslationFromVector(posInverse));
}
