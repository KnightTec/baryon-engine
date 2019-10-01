#include "Engine.h"
#include "GraphicsDeviceInterface.h"
#include "PassKey.h"

#include "windows.h"
#include "Input.h"
#include "Components.h"
#include "EntityManager.h"

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

	return true;
}

void Engine::run()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Input::handleGameInput();
		renderer.render();
	}
	terminate();
}

void Engine::terminate()
{
	renderer.terminate();
	GraphicsDeviceInterface::terminate(Key<Engine>());
}

