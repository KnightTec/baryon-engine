#pragma once
#include "GraphicsDeviceInterface.h"
#include "VirtualScreen.h"

#include <vector>
#include "../Entity.h"

namespace Baryon
{
class VirtualScreen;
class Mesh;
/*
 * The core class of the rendering system
 */
class Renderer : GraphicsDeviceInterface
{
public:
	bool initialize();
	void terminate();

	bool createVirtualScreen(Window& targetWindow);
	void submitMesh(Mesh* mesh);
	void submitEntity(Entity* entity);
	void render();
private:
	std::vector<const Mesh*> meshes;
	std::vector<Entity*> entities;
	std::vector<VirtualScreen> virtualScreens;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementations 
///////////////////////////////////////////////////////////////////////////////////////////////
inline void Renderer::submitMesh(Mesh* mesh)
{
	meshes.push_back(mesh);
}
inline void Renderer::submitEntity(Entity* entity)
{
	entities.push_back(entity);
}


}
