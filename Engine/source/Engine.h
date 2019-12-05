#pragma once
#include "EntityManager.h"
#include "RenderingEngine.h"


namespace Baryon
{
class Window;

/*
 * The core class of the Baryon Engine
 */
class Engine
{
public:
	Engine();
	/*
	 * Must be called before any other function
	 */
	bool initialize();
	void mainLoopIteration();
	void terminate();
	void createVirtualScreen(Window& targetWindow);
	//Renderer& getRenderer();
private:
	//Renderer renderer;
	EntityManager em;
	RenderingEngine renderingEngine;
};

inline Engine::Engine() : renderingEngine(&em)
{
}


//inline Renderer& Engine::getRenderer()
//{
//	return renderer;
//}
}
