#pragma once
#include "GraphicsDeviceInterface.h"

#include <vector>

namespace Baryon
{
class VirtualScreen;

class Renderer : private GraphicsDeviceInterface
{
public:
	void bindVirtualScreen(VirtualScreen* virtualScreen);
	void render();
private:
	std::vector<VirtualScreen*> virtualScreens;
};

inline void Renderer::bindVirtualScreen(VirtualScreen* virtualScreen)
{
	virtualScreens.push_back(virtualScreen);
}
}
