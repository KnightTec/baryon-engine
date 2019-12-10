#include "Engine.h"
#include "GraphicsDeviceInterface.h"
#include "windows.h"
#include "Input.h"
#include "Window.h"
#include "components/Transform.h"

#include "StringId.h"

using namespace Baryon;


bool Engine::initialize()
{
	SetProcessDPIAware();

	Input::initialize();
	ComponentRegistry::initialize();

	if (!GraphicsDeviceInterface::initialize())
	{
		MessageBoxW(nullptr, L"Error: Failed to initialize GraphicsDeviceInterface.", L"Baryon Engine",
		            MB_OK | MB_ICONERROR);
		return false;
	}
	renderingEngine.initialize();

	EntityId id0 = em.createEntity();
	auto* t = em.getComponent<Transform>(id0);
	t->scale(5, 30, 4);
	em.addComponent<StaticMesh>(id0);
	auto* mesh = em.getComponent<StaticMesh>(id0);
	mesh->mesh = new Mesh;


	return true;
}

void Engine::mainLoopIteration()
{
	Input::handleGameInput();
	renderingEngine.render();
	//renderer.render();
}


void Engine::terminate()
{
	//renderer.terminate();
	renderingEngine.terminate();
	GraphicsDeviceInterface::terminate();
}

void Engine::createVirtualScreen(Window& targetWindow)
{
	renderingEngine.createVirtualScreen(targetWindow);
}


