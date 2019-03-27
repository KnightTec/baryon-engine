#pragma once
#include "Renderer.h"

namespace Baryon
{
class Engine
{
public:
	// must be called before any other function
	bool initialize();
	void run();
	Renderer& getRenderer();
private:
	Renderer renderer;
};

inline Renderer& Engine::getRenderer()
{
	return renderer;
}
}