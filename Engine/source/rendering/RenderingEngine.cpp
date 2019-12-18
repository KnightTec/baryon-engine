#include "RenderingEngine.h"
#include "Window.h"
#include "ConstantBuffer.h"
#include "GpuData.h"

using namespace Baryon;


void RenderingEngine::initialize()
{
	ConstantBuffer<PER_OBJECT_DATA>::get().initialize(0, VERTEX_SHADER);
	ConstantBuffer<PER_CAMERA_DATA>::get().initialize(1, PIXEL_SHADER);
	matrixSys.initialize();
	drawSys.initialize();
}
void RenderingEngine::terminate()
{
	drawSys.terminate();
	matrixSys.terminate();
	for (VirtualScreen& screen : virtualScreens)
	{
		screen.terminate();
	}
}
void RenderingEngine::render()
{
	matrixSys.tick();
	cameraSys.tick();
	drawSys.tick();
}
bool RenderingEngine::createVirtualScreen(Window& targetWindow)
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