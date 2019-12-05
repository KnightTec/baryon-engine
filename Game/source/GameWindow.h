#pragma once
#include "WindowsApplication.h"
#include "Window.h"
#include "Size.h"

#include <windows.h>


namespace Baryon
{
class Renderer;

class GameWindow : public WindowsEventHandler, public Window
{
public:
	enum STYLE
	{
		WINDOWED = 0,
		BORDERLESS = 1,
		FULLSCREEN = 2,
	};

	GameWindow(const wchar_t* title, Size2D resolution);
	
	void setStyle(STYLE newStyle);
	void show();

	void setResolution(Size2D resolution) override;
	bool handleEvent(HWND hWnd, UINT uMSg, WPARAM wParam, LPARAM lParam) override;
private:
	/*
	 * Resize the client area of the window
	 */
	void resize(Size2D newSize);
	/*
	 * Trap the cursor inside the client area
	 */
	void trapCursor();

	STYLE style;
};


inline void GameWindow::show()
{
	ShowWindow(hwnd, SW_SHOW);
}

}
