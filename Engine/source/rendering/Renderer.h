#pragma once
#include "GraphicsDeviceInterface.h"
#include "VirtualScreen.h"

#include <vector>

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

	//TODO: use factory for window
	bool createVirtualScreen(Window& targetWindow);
	void submitMesh(const Mesh& mesh);
	void render();
private:
	std::vector<const Mesh*> meshes;
	std::vector<VirtualScreen> virtualScreens;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementations 
///////////////////////////////////////////////////////////////////////////////////////////////
inline void Renderer::submitMesh(const Mesh& mesh)
{
	meshes.push_back(&mesh);
}

}
