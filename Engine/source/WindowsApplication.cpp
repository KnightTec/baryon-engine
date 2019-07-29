#include "WindowsApplication.h"
#include "Input.h"
#include <cassert>
#include "Windowsx.h"

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

	return RegisterClassW(&wc) && getWindowHandle();
}

void WindowsApplication::registerEventHandler(WindowsEventHandler* eventHandler)
{
	eventHandlers.push_back(eventHandler);
}

HWND WindowsApplication::getWindowHandle()
{
	assert(initialized);
	return CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
}

DirectX::XMUINT2 WindowsApplication::getDisplayResolution()
{
	return { static_cast<uint32_t>(GetSystemMetrics(SM_CXSCREEN)), static_cast<uint32_t>(GetSystemMetrics(SM_CYSCREEN)) };
}

LRESULT CALLBACK WindowsApplication::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	assert(initialized);
	switch (uMsg)
	{
	case WM_CLOSE:
		eventHandlers.clear();
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_INPUT:
		Input::passOSData(wParam, lParam);
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
