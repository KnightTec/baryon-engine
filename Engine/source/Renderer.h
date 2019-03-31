#pragma once
#include "GraphicsDeviceInterface.h"

#include <vector>

namespace Baryon
{
class VirtualScreen;
class Mesh;
/*
 * The core class of the rendering system
 */
class Renderer : private GraphicsDeviceInterface
{
public:
	bool initialize();
	void bindVirtualScreen(VirtualScreen* virtualScreen);
	void submitMesh(const Mesh& mesh);
	void render();
private:
	std::vector<const Mesh*> meshes;
	std::vector<VirtualScreen*> virtualScreens;
};

inline void Renderer::bindVirtualScreen(VirtualScreen* virtualScreen)
{
	virtualScreens.push_back(virtualScreen);
}
inline void Renderer::submitMesh(const Mesh& mesh)
{
	meshes.push_back(&mesh);
}

}
