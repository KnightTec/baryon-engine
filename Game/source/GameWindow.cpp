#include "GameWindow.h"
#include "../../Engine/source/Renderer.h"

using namespace Baryon;

GameWindow::GameWindow()
{
}

bool GameWindow::initialize(const wchar_t* name, Renderer* renderer, DirectX::XMUINT2 resolution, STYLE style)
{
	assert(!hwnd);
	hwnd = WindowsApplication::createEmptyWindow();
	if (!hwnd)
	{
		MessageBoxW(nullptr, L"Error: Failed to create window.", L"Baryon Engine", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!screen.initialize(*this))
	{
		MessageBoxW(nullptr, L"Error: Failed to initialize VirtualScreen.", L"Baryon Engine", MB_OK | MB_ICONERROR);
		return false;
	}
	WindowsApplication::registerEventHandler(this);
	SetWindowTextW(hwnd, name);

	setResolution(resolution);
	setStyle(style);

	renderer->bindVirtualScreen(&screen);
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
	if (newStyle != style) {
		SetWindowLongW(hwnd, GWL_STYLE, styles[newStyle]);
		if (style != FULLSCREEN)
		{
			if (newStyle == FULLSCREEN) {
				resize(WindowsApplication::getDisplayResolution());
				screen.setFullscreen(true);
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
				screen.setFullscreen(false);
				resize(resolution);
			}
		}
		style = newStyle;
	}
}

void GameWindow::setResolution(DirectX::XMUINT2 resolution)
{
	this->resolution = resolution;
	if (style != FULLSCREEN)
	{
		resize(resolution);
	}
	screen.resize(resolution);

	//TODO: test resolution switching
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
	case WM_SIZE:
		// resize virtual screen
		//screen.resize({LOWORD(lParam), HIWORD(lParam)});
		return false;
	case WM_ACTIVATE:
		//TODO: handle fullscreen focus
		return false;
	default:
		// message not handled here
		return false;
	}
}
