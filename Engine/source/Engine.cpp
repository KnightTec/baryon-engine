#include "Engine.h"
#include "GraphicsDeviceInterface.h"
#include "PassKey.h"

#include "windows.h"

using namespace Baryon;

bool Engine::initialize()
{
	SetProcessDPIAware();
	if (!GraphicsDeviceInterface::initialize(Key<Engine>()))
	{
		MessageBoxW(nullptr, L"Error: Failed to initialize GraphicsDeviceInterface.", L"Baryon Engine", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}
void Engine::run()
{
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			renderer.render();
		}
	}
}
