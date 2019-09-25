#include "GameWindow.h"
#include "VirtualScreen.h"

using namespace Baryon;

GameWindow::GameWindow(const wchar_t* name, DirectX::XMUINT2 resolution = VirtualScreen::getSupportedResolutions()[0])
	: Window(resolution), style{WINDOWED}
{
	WindowsApplication::registerEventHandler(this);
	SetWindowTextW(hwnd, name);
}

void GameWindow::setStyle(STYLE newStyle)
{
	assert(hwnd);
	static DWORD styles[] = {
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // windowed
		WS_POPUP, // borderless
		WS_POPUP, // fullscreen
	};
	SetWindowLongW(hwnd, GWL_STYLE, styles[newStyle]);
	if (style == newStyle)
	{
		return;
	}
	if (newStyle == FULLSCREEN)
	{
		resize(WindowsApplication::getDisplayResolution());
		screen->setFullscreen(true);
	}
	else
	{
		screen->setFullscreen(false);
		resize(resolution);
	}
	style = newStyle;
}

void GameWindow::setResolution(DirectX::XMUINT2 resolution)
{
	Window::setResolution(resolution);
	resize(resolution);
}

void GameWindow::resize(DirectX::XMUINT2 clientSize)
{
	assert(hwnd);
	const DirectX::XMINT2 screenSize = {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
	const DirectX::XMINT2 newSize{static_cast<int32_t>(clientSize.x), static_cast<int32_t>(clientSize.y)};

	RECT rect;
	rect.left = 0;
	rect.right = newSize.x;
	rect.top = 0;
	rect.bottom = newSize.y;
	const DWORD dwStyle = GetWindowLongW(hwnd, GWL_STYLE);
	AdjustWindowRect(&rect, dwStyle, false);

	// Position window at the center of the screen
	DirectX::XMINT2 position = DirectX::XMINT2{(screenSize.x - newSize.x) / 2, (screenSize.y - newSize.y) / 2};

	// If the window is larger than the screen, the top left corner of the window will stay visible on the screen
	if (newSize.x <= screenSize.x && newSize.y <= screenSize.y)
	{
		position.x += rect.left;
		position.y += rect.top;
	}
	if (position.x < 0)
	{
		position.x = rect.left;
	}
	if (position.y < 0)
	{
		position.y = 0;
	}
	SetWindowPos(hwnd, nullptr, position.x, position.y, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW | SWP_NOZORDER);

	trapCursor();
}

void GameWindow::trapCursor()
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	POINT ul;
	ul.x = rect.left;
	ul.y = rect.top;

	POINT lr;
	lr.x = rect.right;
	lr.y = rect.bottom;

	MapWindowPoints(hwnd, nullptr, &ul, 1);
	MapWindowPoints(hwnd, nullptr, &lr, 1);

	rect.left = ul.x;
	rect.top = ul.y;
	rect.right = lr.x;
	rect.bottom = lr.y;

	ClipCursor(&rect);
}

bool GameWindow::handleEvent(HWND hWnd, UINT uMSg, WPARAM wParam, LPARAM lParam)
{
	switch (uMSg)
	{
	case WM_ACTIVATE:
		trapCursor();
		if (style != FULLSCREEN)
		{
			return false;
		}
		if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
		{
			screen->setFullscreen(true);
			return true;
		}
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			screen->setFullscreen(false);
			return true;
		}
	default:
		// message not handled here
		return false;
	}
}
