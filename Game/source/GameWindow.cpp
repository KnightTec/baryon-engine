#include "GameWindow.h"
#include "../../Engine/source/Renderer.h"

using namespace Baryon;

bool GameWindow::initialize(const wchar_t* name, Renderer* renderer, DirectX::XMUINT2 clientSize, STYLE style)
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
		return false;
	}
	WindowsApplication::registerEventHandler(this);
	SetWindowTextW(hwnd, name);
	setStyle(style);
	resize(clientSize);
	renderer->bindVirtualScreen(&screen);
	return true;
}

DirectX::XMUINT2 GameWindow::getClientSize() const
{
	assert(hwnd);
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	return {static_cast<uint32_t>(clientRect.right), static_cast<uint32_t>(clientRect.bottom)};
}

void GameWindow::setStyle(STYLE newStyle)
{
	assert(hwnd);
	static DWORD styles[3] = {
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // windowed
		WS_POPUP, // borderless
		WS_OVERLAPPEDWINDOW // windowed scalable
	};
	SetWindowLongW(hwnd, GWL_STYLE, styles[newStyle]);
	ShowWindow(hwnd, SW_SHOW);
}

void GameWindow::resize(DirectX::XMUINT2 newClientSize)
{
	assert(hwnd);
	const DirectX::XMINT2 screenSize = {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
	const DirectX::XMINT2 newSize{static_cast<int32_t>(newClientSize.x), static_cast<int32_t>(newClientSize.y)};

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
	SetWindowPos(hwnd, 0, position.x, position.y, rect.right - rect.left, rect.bottom - rect.top,
	             SWP_SHOWWINDOW | SWP_NOZORDER);
}

bool GameWindow::handleEvent(HWND hWnd, UINT uMSg, WPARAM wParam, LPARAM lParam)
{
	switch (uMSg)
	{
	case WM_SIZE:
		// resize virtual screen
		screen.resize({LOWORD(lParam), HIWORD(lParam)});

		return true;
	default:
		// message not handled here
		return false;
	}
}
