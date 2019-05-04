#include "WindowsApplication.h"
#include "Input.h"
#include <cassert>

using namespace Baryon;

bool WindowsApplication::initialized = false;
WNDCLASS WindowsApplication::wc = {};
std::vector<WindowsEventHandler*> WindowsApplication::eventHandlers;

bool WindowsApplication::initialize(HINSTANCE hInstance)
{
	assert(!initialized);
	initialized = true;

	wc.hInstance = hInstance;
	wc.lpfnWndProc = windowProc;
	wc.lpszClassName = L"BaryonWindow";
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));

	return static_cast<bool>(RegisterClassW(&wc));
}

void WindowsApplication::registerEventHandler(WindowsEventHandler* eventHandler)
{
	assert(initialized);
	eventHandlers.push_back(eventHandler);
}

HWND WindowsApplication::createEmptyWindow()
{
	assert(initialized);
	return CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
}

LRESULT CALLBACK WindowsApplication::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	assert(initialized);
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		Input::handleOSinput(wParam, 1);
		return 0;
	case WM_KEYUP:
		Input::handleOSinput(wParam, 0);
		return 0;
	default:
		bool handledMessage = false;
		for (WindowsEventHandler* eventHandler : eventHandlers)
		{
			if (eventHandler->handleEvent(hWnd, uMsg, wParam, lParam))
			{
				handledMessage = true;
			}
		}
		if (handledMessage)
		{
			return 0;
		}
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
}
