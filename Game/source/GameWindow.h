#pragma once
#include "../../Engine/source/WindowsApplication.h"
#include "../../Engine/source/rendering/Window.h"

#include <windows.h>
#include "DirectXMath.h"


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

	GameWindow(const wchar_t* name, DirectX::XMUINT2 resolution);
	
	void setStyle(STYLE newStyle);
	void setResolution(DirectX::XMUINT2 resolution) override;
	bool handleEvent(HWND hWnd, UINT uMSg, WPARAM wParam, LPARAM lParam) override;
private:
	/*
	 * Resize the client area of the window
	 */
	void resize(DirectX::XMUINT2 clientSize);
	/*
	 * Trap the cursor inside the client area
	 */
	void trapCursor();

	STYLE style;
};

}
