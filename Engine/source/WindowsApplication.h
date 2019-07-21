#pragma once
#include "windows.h"
#include <vector>
#include <DirectXMath.h>

namespace Baryon
{
class WindowsEventHandler
{
public:
	virtual ~WindowsEventHandler() = default;
	/*
	 * The return value indicates whether the message has been handled
	 */
	virtual bool handleEvent(HWND, UINT, WPARAM, LPARAM) = 0;
};

class WindowsApplication
{
public:
	static bool initialize(HINSTANCE hInstance);
	/*
	 * Register a function that handles Windows messages
	 */
	static void registerEventHandler(WindowsEventHandler* eventHandler);
	/*
	 * Create a unspecified window
	 */
	static HWND getWindowHandle();
	static DirectX::XMUINT2 getDisplayResolution();
private:
	static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static bool initialized;
	static WNDCLASS wc;
	static std::vector<WindowsEventHandler*> eventHandlers;
};
}
