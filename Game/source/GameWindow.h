#pragma once
#include "../../Engine/source/WindowsApplication.h"
#include "../../Engine/source/Window.h"

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

	GameWindow(const wchar_t* name, DirectX::XMUINT2 resolution, STYLE style);
	
	void setStyle(STYLE newStyle);
	/*
	 * Set the resolution of the back buffer
	 */
	void setResolution(DirectX::XMUINT2 resolution);

	bool handleEvent(HWND hWnd, UINT uMSg, WPARAM wParam, LPARAM lParam) override;
private:
	bool initialize() override;
	/*
	 * Resize the windows client area
	 */
	void resize(DirectX::XMUINT2 clientSize);

	STYLE style;

	//TODO: move this member to Window.h
	DirectX::XMUINT2 resolution;
};

}
