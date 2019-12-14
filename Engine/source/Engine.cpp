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

	EntityId id0 = em.createEntity();
	em.addComponent<WorldMatrixComponent>(id0);
	auto* t = em.getComponent<Transform>(id0);
	t->translate(0, 10, 0);
	em.addComponent<MeshComponent>(id0);
	auto* mesh = em.getComponent<MeshComponent>(id0);

	
	Mesh::import("../../untitled.obj");

	StringId meshPath("mesh.bass");
	mesh->mesh = ResourceManager::get().getMesh(meshPath);

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


