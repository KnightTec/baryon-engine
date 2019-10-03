#pragma once
#include "Renderer.h"
#include "EntityManager.h"


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
	void terminate();
	Renderer& getRenderer();
private:
	Renderer renderer;
	EntityManager em;
};

inline Renderer& Engine::getRenderer()
{
	return renderer;
}

}
