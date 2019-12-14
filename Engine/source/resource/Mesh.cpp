#include "Mesh.h"
#include "IOUtil.h"

#include "DirectXMath.h"
#include <minwinbase.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <map>

using namespace Baryon;
using namespace DirectX;

namespace
{
	// values needed for the binary file
	const char* resTypeId = "BarResMesh";
	const uint16_t resTypeIdLength = strlen(resTypeId) + 1;
	const uint16_t version = 1;
	const uint16_t vertexSize = sizeof(Vertex);
}

Mesh::Mesh() : indexCount(0)
{
}

bool Mesh::load(const char* filename)
{
	using namespace IOUtil;

	std::ifstream file(filename, std::ios_base::binary);
	if (!file.good())
	{
		return false;
	}

	uint16_t length;
	readValue(file, length);
	if (length != resTypeIdLength)
	{
		return false;
	}
	char resTypeIdBuf[32];
	file.read(resTypeIdBuf, length);
	if (strcmp(resTypeId, resTypeIdBuf))
	{
		return false;
	}

	//TODO: CRC-32 checksum


	uint16_t version;
	readValue(file, version);
	uint32_t vertexOffset, indexOffset;
	readValue(file, vertexOffset);
	readValue(file, indexOffset);
	uint16_t vertexSize;
	readValue(file, vertexSize);
	uint32_t vertexCount, indexCount;
	readValue(file, vertexCount);
	readValue(file, indexCount);

	this->indexCount = indexCount;

	size_t indexSize = sizeof(uint32_t);
	auto vertexData = static_cast<uint8_t*>(malloc(vertexCount * vertexSize));
	auto indexData = static_cast<uint32_t*>(malloc(indexCount * indexSize));

	file.seekg(vertexOffset);
	file.read(reinterpret_cast<char*>(vertexData), vertexCount * vertexSize);
	file.seekg(indexOffset);
	file.read(reinterpret_cast<char*>(indexData), indexCount * indexSize);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertexCount);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertexData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	getDevice()->CreateBuffer(&bufferDesc, &initData, vertexBuffer.GetAddressOf());

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = indexSize * indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indexData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	getDevice()->CreateBuffer(&bufferDesc, &initData, indexBuffer.GetAddressOf());

	free(vertexData);
	free(indexData);

	return true;
}

/*
 * import a .obj-file into a custom binary format
 */
void Mesh::import(const char* source)
{
	std::ifstream inputFile(source);

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texCoords;

	struct FaceIndex
	{
		int position;
		int texCoord;
		int normal;
	};
	struct Face
	{
		FaceIndex index[3] = {};
	};
	std::vector<Face> faces;

	while (inputFile.good())
	{
		std::string line;
		if (!std::getline(inputFile, line))
		{
			break;
		}
		std::stringstream ss(line);
		ss.ignore(8, ' ');
		switch (line[0])
		{
		case 'v':
			switch (line[1])
			{
			case ' ':
				positions.emplace_back();
				ss >> positions.back().x >> positions.back().y >> positions.back().z;
				break;
			case 'n':
				normals.emplace_back();
				ss >> normals.back().x >> normals.back().y >> normals.back().z;
				// normals might not be normalized
				XMStoreFloat3(&normals.back(), XMVector3Normalize(XMLoadFloat3(&normals.back())));
				break;
			case 't':
				texCoords.emplace_back();
				ss >> texCoords.back().x >> texCoords.back().y;
				break;
			default:
				break;
			}
			break;
		case 'f':
			faces.emplace_back();
			for (auto& index : faces.back().index)
			{
				ss >> index.position;
				index.position -= 1;
				if (ss.peek() != '/')
				{
					break;
				}
				ss.ignore(1);
				if (ss.peek() != '/')
				{
					ss >> index.texCoord;
					index.texCoord -= 1;
				}
				if (ss.peek() != '/')
				{
					break;
				}
				ss.ignore(1);
				ss >> index.normal;
				index.normal -= 1;
			}
			break;
		case '#':
		default:
			break;
		}
	}
	inputFile.close();


	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	size_t faceCount = faces.size();
	uint32_t nextIndex = 0;
	std::map<std::tuple<int, int, int>, uint32_t> indexMap;
	for (size_t i = 0; i < faceCount; i++)
	{
		for (int j = 2; j >= 0; j--)
		{
			auto& index = faces[i].index[j];
			uint32_t newIndex;
			auto tup = std::make_tuple(index.position, index.normal, index.texCoord);
			auto it = indexMap.find(tup);
			if (it != indexMap.end())
			{
				newIndex = it->second;
			}
			else
			{
				newIndex = nextIndex++;
				indexMap[tup] = newIndex;
				vertices.emplace_back(Vertex{});
				vertices.back().position = positions[index.position];
				if (normals.size() > index.normal)
				{
					vertices.back().normal = normals[index.normal];
				}
				if (texCoords.size() > index.texCoord)
				{
					vertices.back().texCoord = texCoords[index.texCoord];
				}
			}
			indices.emplace_back(newIndex);
		}
	}
	uint32_t vertexCount = vertices.size();
	uint32_t indexCount = indices.size();

	using namespace IOUtil;

	// write to file
	std::ofstream outputFile("mesh.bass", std::ios::binary);
	writeValue(outputFile, resTypeIdLength);
	outputFile.write(resTypeId, resTypeIdLength);

	writeValue(outputFile, version);

	uint32_t vertexOffset = sizeof resTypeIdLength;
	vertexOffset += resTypeIdLength * sizeof resTypeId[0];
	vertexOffset += sizeof uint32_t * 4 + sizeof uint16_t * 2;
	writeValue(outputFile, vertexOffset);

	uint32_t indexOffset = vertexOffset + vertexCount * vertexSize;
	writeValue(outputFile, indexOffset);
	writeValue(outputFile, vertexSize);
	writeValue(outputFile, vertexCount);
	writeValue(outputFile, indexCount);

	outputFile.write(reinterpret_cast<char*>(vertices.data()), vertexCount * vertexSize);
	outputFile.write(reinterpret_cast<char*>(indices.data()), indexCount * sizeof indices[0]);
	outputFile.close();
}
