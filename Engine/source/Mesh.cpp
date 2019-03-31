#include "Mesh.h"
#include "DirectXMath.h"
#include <fstream>
#include <minwinbase.h>

using namespace Baryon;
using namespace DirectX;

Mesh::Mesh()
{
	// create cube
	vertices.emplace_back(Vertex{{1, 1, 1}, {0.5773f, 0.5773f, 0.5773f}});
	vertices.emplace_back(Vertex{{1, 1, -1}, {0.5773f, 0.5773f, -0.5773f}});
	vertices.emplace_back(Vertex{{1, -1, -1}, {0.5773f, -0.5773f, -0.5773f}});
	vertices.emplace_back(Vertex{{1, -1, 1}, {0.5773f, -0.5773f, 0.5773f}});
	vertices.emplace_back(Vertex{{-1, 1, 1}, {-0.5773f, 0.5773f, 0.5773f}});
	vertices.emplace_back(Vertex{{-1, 1, -1}, {-0.5773f, 0.5773f, -0.5773f}});
	vertices.emplace_back(Vertex{{-1, -1, -1}, {-0.5773f, -0.5773f, -0.5773f}});
	vertices.emplace_back(Vertex{{-1, -1, 1}, {-0.5773f, -0.5773f, 0.5773f}});

	indices = {
		// +x
		0, 1, 2,
		0, 2, 3,
		// -x
		7, 5, 4,
		7, 6, 5,
		// +y
		4, 1, 0,
		4, 5, 1,
		// -y
		3, 6, 7,
		3, 2, 6,
		// +z
		4, 3, 7,
		4, 0, 3,
		// -z
		1, 6, 2,
		1, 5, 6
	};

	indexCount = indices.size();

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices.data();
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	getDevice().CreateBuffer(&bufferDesc, &initData, vertexBuffer.GetAddressOf());

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(uint32_t) * indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indices.data();
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	getDevice().CreateBuffer(&bufferDesc, &initData, indexBuffer.GetAddressOf());
}
bool Mesh::initialize(const char* filename)
{
	//std::ifstream stream{filename};
	//if (!stream)
	//{
	//	return false;
	//}
	//char buffer[128];
	//while (stream.good())
	//{
	//	int nextChar = stream.get();
	//	switch (nextChar)
	//	{
	//	case '#':
	//		// ignore comments
	//		stream.ignore(256, '\n');
	//		break;
	//	case 'v':
	//		// parse vertex data
	//		nextChar = stream.get();
	//			switch (nextChar)
	//			{
	//			case ' ':
	//				// vertex position
	//				vertices.emplace_back(Vertex());
	//				Vertex& vertex = vertices.back();
	//				stream >> vertex.position.x >> vertex.position.y >> vertex.position.z;
	//				stream.ignore(256, '\n');
	//				break;
	//			case 'n':
	//				// vertex normal
	//				vertices.emplace_back(Vertex());
	//				Vertex& vertex = vertices.back();
	//				stream >> vertex.position.x >> vertex.position.y >> vertex.position.z;
	//				stream.ignore(256, '\n');
	//				break;
	//			case 't':
	//				// texture coordinates

	//				break;
	//			default:
	//				return false;
	//			}
	//		break;
	//	case 'f':
	//		break;
	//	default:
	//		// invalid file
	//		return false;
	//	}
	//}
	//stream.get();
	////TODO: parse file

	//return true;
	return false;
}
