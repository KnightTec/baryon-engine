#include "GameWindow.h"
#include "../../Engine/source/Renderer.h"
#include "../../Engine/source/VirtualScreen.h"

using namespace Baryon;

GameWindow::GameWindow(const wchar_t* name, DirectX::XMUINT2 resolution = VirtualScreen::supportedResolutions[0], STYLE style = WINDOWED) 
	: style{WINDOWED}, resolution{resolution}
{
	WindowsApplication::registerEventHandler(this);
	SetWindowTextW(hwnd, name);
}

bool GameWindow::initialize()
{

	// TODO: remove init function

	setStyle(style);
	setResolution(resolution);

	return true;
}


void GameWindow::setStyle(STYLE newStyle)
{
	assert(hwnd);
	static DWORD styles[] = {
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // windowed
		WS_POPUP, // borderless
		WS_POPUP, // fullscreen
		WS_OVERLAPPEDWINDOW, // windowed scalable
	};
	SetWindowLongW(hwnd, GWL_STYLE, styles[newStyle]);
	if (style != FULLSCREEN)
	{
		if (newStyle == FULLSCREEN)
		{
			resize(WindowsApplication::getDisplayResolution());
			screen->setFullscreen(true);
		}
		else
		{
			resize(resolution);
		}
	}
	else
	{
		if (newStyle != FULLSCREEN)
		{
			screen->setFullscreen(false);
			resize(resolution);
		}
	}
	style = newStyle;
}

void GameWindow::setResolution(DirectX::XMUINT2 resolution)
{
	this->resolution = resolution;
	if (style != FULLSCREEN)
	{
		resize(resolution);
	}
	screen->resize(resolution);
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
	SetWindowPos(hwnd, nullptr, position.x, position.y, rect.right - rect.left, rect.bottom - rect.top,
	             SWP_SHOWWINDOW | SWP_NOZORDER);
}

bool GameWindow::handleEvent(HWND hWnd, UINT uMSg, WPARAM wParam, LPARAM lParam)
{
	switch (uMSg)
	{
		case WM_ACTIVATE:
			if (style != FULLSCREEN)
			{
				return false;
			}
			if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
			{
				resize(WindowsApplication::getDisplayResolution());
				screen->setFullscreen(true);
				return true;
			}
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				screen->setFullscreen(false);
				ShowWindow(hwnd, SW_MINIMIZE);
				return true;
			}
		default:
			// message not handled here
			return false;
	}
}
