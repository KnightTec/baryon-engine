#pragma once
#include "../../Engine/source/WindowsApplication.h"
#include "../../Engine/source/VirtualScreen.h"
#include "../../Engine/source/Window.h"

#include <windows.h>
#include "DirectXMath.h"


namespace Baryon
{
class Renderer;

class GameWindow : public WindowsEventHandler, Window
{
public:
	enum STYLE
	{
		WINDOWED = 0,
		BORDERLESS = 1,
		FULLSCREEN = 2,
		//WINDOWED_SCALABLE = 3 //TODO: remove this style
	};

	GameWindow();
	bool initialize(const wchar_t* name, Renderer* renderer, DirectX::XMUINT2 resolution, STYLE style);

	void setStyle(STYLE newStyle);
	/*
	 * Set the resolution of the back buffer
	 */
	void setResolution(DirectX::XMUINT2 resolution);
	void setActiveCamera(Camera* camera);

	bool handleEvent(HWND hWnd, UINT uMSg, WPARAM wParam, LPARAM lParam) override;
private:
	/*
	 * Resize the windows client area
	 */
	void resize(DirectX::XMUINT2 clientSize);

	STYLE style;
	DirectX::XMUINT2 resolution;
	VirtualScreen screen;
};


inline void GameWindow::setActiveCamera(Camera* camera)
{
	screen.setActiveCamera(camera);
}
}
