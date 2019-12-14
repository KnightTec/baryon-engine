#include "ResourceManager.h"

using namespace Baryon;

ResourceManager::~ResourceManager()
{
	for (auto& pair : meshes)
	{
		pair.second->~Mesh();
	}
}
Mesh* ResourceManager::getMesh(StringId path)
{
	uint64_t pathHash = path.getHash();
	auto it = meshes.find(pathHash);
	if (it == meshes.end())
	{
		Mesh* newMesh = new (meshPool.allocate()) Mesh;
		if (newMesh->load(path.c_str()))
		{
			meshes[pathHash] = newMesh;
		}
		else
		{
			meshes[pathHash] = nullptr;
		}
	}
	return meshes[pathHash];
}
