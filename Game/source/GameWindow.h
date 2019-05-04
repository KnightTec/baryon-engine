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
		WINDOWED_SCALABLE = 2 //TODO: remove this style
		//TODO: FULLSCREEN
	};

	GameWindow() = default;
	bool initialize(const wchar_t* name, Renderer* renderer, DirectX::XMUINT2 clientSize, STYLE style);

	DirectX::XMUINT2 getClientSize() const;
	void setStyle(STYLE newStyle);
	void resize(DirectX::XMUINT2 newClientSize);
	void setActiveCamera(Camera* camera);

	bool handleEvent(HWND hWnd, UINT uMSg, WPARAM wParam, LPARAM lParam) override;
private:
	VirtualScreen screen;
};


inline void GameWindow::setActiveCamera(Camera* camera)
{
	screen.setActiveCamera(camera);
}
}
