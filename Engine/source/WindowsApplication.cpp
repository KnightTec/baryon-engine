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

	return RegisterClassW(&wc) && createEmptyWindow();
}

void WindowsApplication::registerEventHandler(WindowsEventHandler* eventHandler)
{
	eventHandlers.push_back(eventHandler);
}

HWND WindowsApplication::createEmptyWindow()
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
	int xPosAbsolute;
	int yPosAbsolute;
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
	case WM_MOUSEMOVE:
		xPosAbsolute = GET_X_LPARAM(lParam);
		yPosAbsolute = GET_Y_LPARAM(lParam);
		return 0;
	case WM_INPUT:
		Input::handleOSInput(wParam, lParam);
		return 0;
	case WM_KEYDOWN:
		//Input::handleOSInputOld(wParam, 1);
		return 0;
	case WM_KEYUP:
		//Input::handleOSInputOld(wParam, 0);
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
