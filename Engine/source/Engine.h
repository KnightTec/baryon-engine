#pragma once
#include "Renderer.h"

namespace Baryon
{
/*
 * The core class of the Baryon Engine
 */
class Engine
{
public:
	/*
	 * Must be called before any other function
	*/
	bool initialize();
	/*
	 * Run the main loop of the game engine
	 */
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
