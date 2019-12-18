#pragma once
#include "EntityManager.h"
#include "WorldMatrixSystem.h"
#include "CameraSystem.h"
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
	WorldMatrixSystem matrixSys;
	CameraSystem cameraSys;
	DrawingSystem drawSys;

	std::vector<VirtualScreen> virtualScreens;
};

inline RenderingEngine::RenderingEngine(EntityManager* entityManager)
	: matrixSys(entityManager), cameraSys(entityManager), drawSys(entityManager, virtualScreens)
{
}
}
