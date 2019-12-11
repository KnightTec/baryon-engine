#pragma once
#include "GraphicsDeviceInterface.h"

#include <DirectXMath.h>
#include "wrl/client.h"

#include <cstdint>
#include <string>

namespace Baryon
{
struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texCoord;
};

class Mesh : GraphicsDeviceInterface
{
public:
	Mesh();
	bool load(const char* filename);

	// returns the path of the converted mesh
	static void import(const char* source);

	ID3D11Buffer* getVertexBuffer() const;
	ID3D11Buffer* getIndexBuffer() const;
	uint32_t getIndexCount() const;
private:
	uint32_t indexCount;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	static const char* resTypeId;
	static const uint16_t resTypeIdLength;
	static const uint16_t version;
	static const uint16_t vertexSize;
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
