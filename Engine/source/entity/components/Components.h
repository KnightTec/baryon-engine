#pragma once
#include "Transform.h"
#include "Mesh.h"


namespace Baryon
{
struct WorldMatrixComponent
{
	DirectX::XMFLOAT4X3 worldMatrix;
};
struct MeshComponent
{
	
	Mesh* mesh;
};
}
