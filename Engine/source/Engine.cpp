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

	for (int x = 0; x < 20; x++)
	{
		for (int y = 0; y < 20; y++) {
			EntityId id = em.createEntity();
			em.addComponents<WorldMatrixComponent, MeshComponent>(id);
			auto* t = em.getComponent<Transform>(id);
			t->translate(x * 14, 10, y * 14);
			t->scale(5, 5, 5);
			auto* m = em.getComponent<MeshComponent>(id);
			m->mesh = ResourceManager::get().getMesh(meshPath);
		}
	}

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


