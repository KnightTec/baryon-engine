#pragma once
#include "EntityManager.h"
#include "WorldMatrixCalculatorSystem.h"
#include "DrawingSystem.h"
#include "VirtualScreen.h"

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
}
