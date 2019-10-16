#include "Engine.h"
#include "GraphicsDeviceInterface.h"
#include "PassKey.h"

#include "windows.h"
#include "Input.h"
#include "Components.h"


using namespace Baryon;



bool Engine::initialize()
{
	SetProcessDPIAware();

	Input::initialize();
	ComponentRegistry::initialize();

	if (!GraphicsDeviceInterface::initialize(Key<Engine>()))
	{
		MessageBoxW(nullptr, L"Error: Failed to initialize GraphicsDeviceInterface.", L"Baryon Engine",
		            MB_OK | MB_ICONERROR);
		return false;
	}
	if (!renderer.initialize())
	{
		MessageBoxW(nullptr, L"Error: Failed to initialize Renderer.", L"Baryon Engine", MB_OK | MB_ICONERROR);
		return false;
	}

	EntityId id = em.createEntity();
	em.addComponent<StaticMesh>(id);
	auto* t = em.getComponent<Transform>(id);
	t->position.x = 1;
	t = em.getComponent<Transform>(id);
	id = em.createEntity();
	em.addComponent<StaticMesh>(id);


	return true;
}

void Engine::mainLoopIteration()
{
	Input::handleGameInput();
	tst.tick();
	renderer.render();
}


void Engine::terminate()
{
	renderer.terminate();
	GraphicsDeviceInterface::terminate(Key<Engine>());
}

