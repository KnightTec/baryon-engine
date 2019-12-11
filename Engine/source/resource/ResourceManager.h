#pragma once
#include <unordered_map>
#include "StringId.h"
#include "Mesh.h"

//#include "Prototype.h"

namespace Baryon
{
class ResourceManager
{
public:
	Mesh* getMesh(StringId path);
private:
	 std::unordered_map<StringId, Mesh> meshes;
	// Meshes
	// Textures
	// Materials

};
}
