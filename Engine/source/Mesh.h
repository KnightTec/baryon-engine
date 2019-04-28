#pragma once
#include "GraphicsDeviceInterface.h"

#include <DirectXMath.h>

#include <cstdint>
#include <vector>

namespace Baryon
{
struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
};

class Mesh : GraphicsDeviceInterface
{
public:
	Mesh();
	bool initialize(const char* filename);

	ID3D11Buffer* getVertexBuffer() const;
	ID3D11Buffer* getIndexBuffer() const;
	uint32_t getIndexCount() const;
private:
	uint32_t indexCount;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementations 
///////////////////////////////////////////////////////////////////////////////////////////////
inline ID3D11Buffer* Mesh::getVertexBuffer() const
{
	return vertexBuffer.Get();
}
inline ID3D11Buffer* Mesh::getIndexBuffer() const
{
	return indexBuffer.Get();
}
inline uint32_t Mesh::getIndexCount() const
{
	return indexCount;
}
}
