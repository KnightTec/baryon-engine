#pragma once
#include "VirtualScreen.h"

#include <windows.h>
#include "DirectXMath.h"

namespace Baryon
{
class GameWindow
{
public:
	enum STYLE
	{
		WINDOWED = 0,
		BORDERLESS = 1,
		WINDOWED_SCALABLE = 2 //TODO: remove this style
		//TODO: FULLSCREEN
	};

	GameWindow();
	bool initialize(DirectX::XMUINT2 clientSize, STYLE style);
	DirectX::XMUINT2 getClientSize() const;
	void setStyle(STYLE newStyle);
	void resize(DirectX::XMUINT2 newClientSize);
private:
	HWND windowHandle;
	VirtualScreen* screen;
	friend bool VirtualScreen::initialize(GameWindow& window);
};
}
