#include "Engine.h"

using namespace Baryon;

bool Engine::startUp()
{
	if (!renderingEngine.startUp())
	{
		MessageBoxW(nullptr, L"Error: Failed to initialize rendering Engine.", L"Baryon Engine", MB_OK | MB_ICONERROR);
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
			renderingEngine.render();
		}
	}
}
