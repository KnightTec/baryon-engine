#pragma once
#include "EntityManager.h"
#include "WorldMatrixCalculatorSystem.h"
#include "DrawingSystem.h"
#include "VirtualScreen.h"
#include "Window.h"

#include <vector>

namespace Baryon
{
class RenderingEngine
{
public:
	RenderingEngine(EntityManager* entityManager);
	void initialize();
	void terminate();
	void render();
	bool createVirtualScreen(Window& targetWindow);
private:
	WorldMatrixCalculatorSystem matrixSys;
	DrawingSystem drawSys;

	std::vector<VirtualScreen> virtualScreens;
};


inline RenderingEngine::RenderingEngine(EntityManager* entityManager)
	: matrixSys(entityManager), drawSys(entityManager, virtualScreens)
{
}
inline void RenderingEngine::initialize()
{
	matrixSys.initialize();
	drawSys.initialize();
}
inline void RenderingEngine::terminate()
{
	drawSys.terminate();
	matrixSys.terminate();
	for (VirtualScreen& screen : virtualScreens)
	{
		screen.terminate();
	}
}
inline void RenderingEngine::render()
{
	matrixSys.tick();
	drawSys.tick();
}
inline bool RenderingEngine::createVirtualScreen(Window& targetWindow)
{
	virtualScreens.emplace_back(VirtualScreen{});
	if (!virtualScreens.back().initialize(targetWindow))
	{
		virtualScreens.pop_back();
		return false;
	}
	targetWindow.screen = &virtualScreens.back();
	targetWindow.setResolution(targetWindow.getResolution());
	return true;
}
}
