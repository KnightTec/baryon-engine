#pragma once
#include "Renderer.h"
#include <unordered_set>

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
	void registerUpdateFunction(void(*update)());
private:
	Renderer renderer;
	std::unordered_set<void(*)()> updateFunctions;
};

inline Renderer& Engine::getRenderer()
{
	return renderer;
}
inline void Engine::registerUpdateFunction(void (* update)())
{
	updateFunctions.insert(update);
}

}
