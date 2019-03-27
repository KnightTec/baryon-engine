#include "GameWindow.h"

using namespace Baryon;

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

GameWindow::GameWindow() : windowHandle {nullptr}, screen{nullptr}
{
}
bool GameWindow::initialize(DirectX::XMUINT2 clientSize, STYLE style)
{
	assert(!windowHandle);
	WNDCLASS wc = {};
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"BaryonWindow";
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	RegisterClass(&wc);

	HWND hwnd = CreateWindow(wc.lpszClassName, L"Baryon Game", 0, 0, 0, 0, 0, nullptr,
		nullptr, wc.hInstance, nullptr);
	if (hwnd)
	{
		windowHandle = hwnd;
		setStyle(style);
		resize(clientSize);
		return true;
	}
	MessageBoxW(nullptr, L"Error: Failed to create window.", L"Baryon Engine", MB_OK | MB_ICONERROR);
	return false;
}

DirectX::XMUINT2 GameWindow::getClientSize() const
{
	assert(windowHandle);
	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);
	return {static_cast<uint32_t>(clientRect.right), static_cast<uint32_t>(clientRect.bottom)};
}
void GameWindow::setStyle(STYLE newStyle)
{
	assert(windowHandle);
	static DWORD styles[3] = {
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // windowed
		WS_POPUP, // borderless
		WS_OVERLAPPEDWINDOW // windowed scalable
	};
	SetWindowLong(windowHandle, GWL_STYLE, styles[newStyle]);
	ShowWindow(windowHandle, SW_SHOW);
}
void GameWindow::resize(DirectX::XMUINT2 newClientSize)
{
	assert(windowHandle);
	DirectX::XMINT2 screenSize = {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
	DirectX::XMINT2 newSize{static_cast<int32_t>(newClientSize.x), static_cast<int32_t>(newClientSize.y)};

	RECT rect;
	rect.left = 0;
	rect.right = newSize.x;
	rect.top = 0;
	rect.bottom = newSize.y;
	DWORD dwStyle = GetWindowLong(windowHandle, GWL_STYLE);
	AdjustWindowRect(&rect, dwStyle, false);

	// Position window at the center of the screen
	DirectX::XMINT2 position = DirectX::XMINT2{(screenSize.x - newSize.x) / 2, (screenSize.y - newSize.y) / 2};

	// If window is larger than the screen, the top left corner will stay on the screen
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
	SetWindowPos(windowHandle, 0, position.x, position.y, rect.right - rect.left, rect.bottom - rect.top,
	             SWP_SHOWWINDOW | SWP_NOZORDER);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
