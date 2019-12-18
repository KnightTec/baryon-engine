#pragma once
#include "EntityManager.h"
#include "RenderingEngine.h"
#include "World.h"


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
	void injectVirtualScreen(Window& targetWindow);
	World* createWorld();
private:
	EntityManager entityManager;
	RenderingEngine renderingEngine;
	World world;
};

inline Engine::Engine() : renderingEngine(&entityManager), world(&entityManager)
{
}
inline World* Engine::createWorld()
{
	//TODO: actually create a new world
	return &world;
}



}
