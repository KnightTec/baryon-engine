#include "Engine.h"
#include "GraphicsDeviceInterface.h"
#include "windows.h"
#include "Input.h"
#include "Window.h"
#include "components/Transform.h"

using namespace Baryon;


bool Engine::initialize()
{
	SetProcessDPIAware();

	Input::initialize();

	if (!GraphicsDeviceInterface::initialize())
	{
		MessageBoxW(nullptr, L"Error: Failed to initialize GraphicsDeviceInterface.", L"Baryon Engine",
		            MB_OK | MB_ICONERROR);
		return false;
	}
	renderingEngine.initialize();

	return true;
}

void Engine::mainLoopIteration()
{
	Input::handleGameInput();
	scriptSystem.tick();
	renderingEngine.render();
}


void Engine::terminate()
{
	renderingEngine.terminate();
	GraphicsDeviceInterface::terminate();
}

void Engine::injectVirtualScreen(Window& targetWindow)
{
	renderingEngine.createVirtualScreen(targetWindow);
}


