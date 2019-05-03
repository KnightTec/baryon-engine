#include "Engine.h"
#include "GraphicsDeviceInterface.h"
#include "PassKey.h"

#include "windows.h"
#include "Input.h"

using namespace Baryon;

bool Engine::initialize()
{
	SetProcessDPIAware();
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
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Input::handleGameInput();
			/*for (auto updateFunction : updateFunctions)
			{
				updateFunction();
			}*/
			renderer.render();
		}
	}
}
