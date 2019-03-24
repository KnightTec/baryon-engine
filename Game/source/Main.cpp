#include <windows.h>
#include "../../Engine/source/Window.h"
#include "../../Engine/source/Engine.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Baryon::Window window({ 1600, 900 }, Baryon::Window::WINDOWED_SCALABLE);
	Baryon::Engine engine;
	if (!engine.startUp())
	{
		return EXIT_FAILURE;
	}
	Baryon::Viewport* viewport = nullptr;
	engine.renderingEngine.createViewport(window, viewport);	
	engine.run();
    return EXIT_SUCCESS;
}