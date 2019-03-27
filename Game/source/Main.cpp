#include <windows.h>
#include "../../Engine/source/GameWindow.h"
#include "../../Engine/source/Engine.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Baryon::Engine engine;
	if (!engine.initialize())
	{
		return EXIT_FAILURE;
	}
	Baryon::GameWindow window;
	if (!window.initialize({ 1600, 900 }, Baryon::GameWindow::WINDOWED_SCALABLE))
	{
		return EXIT_FAILURE;
	}

	Baryon::VirtualScreen screen;
	screen.initialize(window);
	engine.getRenderer().bindVirtualScreen(&screen);
	engine.run();
    return EXIT_SUCCESS;
}