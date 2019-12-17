#pragma once
#include "StringId.h"
#include "Mesh.h"
#include "Singleton.h"
#include "Memory.h"

#include <unordered_map>


//#include "Prototype.h"

namespace Baryon
{
class ResourceManager : public Singleton<ResourceManager>
{
	friend class Singleton<ResourceManager>;
public:
	Mesh* getMesh(StringId path);
private:
	ResourceManager() = default;
	~ResourceManager();

	//TODO: add reference counting
	// maps the string hash of the path of the resource to the mesh 
	std::unordered_map<uint64_t, Mesh*> meshes;
	PoolAllocator<Mesh> meshPool;
	// Textures
	// Materials
};
}
