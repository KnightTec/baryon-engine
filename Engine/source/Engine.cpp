#include "Engine.h"
#include "GraphicsDeviceInterface.h"
#include "windows.h"
#include "Input.h"
#include "Window.h"
#include "components/Transform.h"

#include "StringId.h"
#include "ResourceManager.h"

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

	Mesh::import("../../untitled.obj");
	StringId meshPath("mesh.bass");

	EntityId id = em.createEntity();
	em.addComponents<WorldMatrixComponent, MeshComponent>(id);
	auto* t = em.getComponent<Transform>(id);
	t->translate(0, 10, 0);
	t->scale(5, 5, 5);
	em.getComponent<MeshComponent>(id)->mesh = ResourceManager::get().getMesh(meshPath);

	return true;
}

void Engine::mainLoopIteration()
{
	Input::handleGameInput();
	renderingEngine.render();
}


void Engine::terminate()
{
	renderingEngine.terminate();
	GraphicsDeviceInterface::terminate();
}

void Engine::createVirtualScreen(Window& targetWindow)
{
	renderingEngine.createVirtualScreen(targetWindow);
}


