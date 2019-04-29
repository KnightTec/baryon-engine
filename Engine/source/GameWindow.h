#pragma once
#include "VirtualScreen.h"

#include <windows.h>
#include "DirectXMath.h"

namespace Baryon
{
class GameWindow
{
	friend bool VirtualScreen::initialize(GameWindow& window);
public:
	enum STYLE
	{
		WINDOWED = 0,
		BORDERLESS = 1,
		WINDOWED_SCALABLE = 2 //TODO: remove this style
		//TODO: FULLSCREEN
	};

	GameWindow();
	bool initialize(const wchar_t* name, DirectX::XMUINT2 clientSize, STYLE style);
	DirectX::XMUINT2 getClientSize() const;
	void setStyle(STYLE newStyle);
	void resize(DirectX::XMUINT2 newClientSize);

	LRESULT CALLBACK handleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HWND windowHandle;
	VirtualScreen* screen;
};
}
