#pragma once
#include <windows.h>
#include "DirectXMath.h"

namespace Baryon
{
class Window
{
public:
	enum STYLE
	{
		WINDOWED = 0,
		BORDERLESS = 1,
		WINDOWED_UNLOCKED = 2	//TODO: remove this style
	};

	Window(DirectX::XMINT2 clientSize, STYLE style);

	HWND getHwnd() const;
	DirectX::XMINT2 getClientSize() const;
	void setStyle(STYLE newStyle);
	void resize(DirectX::XMINT2 newSize);
private:
	HWND windowHandle;
};

inline HWND Window::getHwnd() const
{
	return windowHandle;
}

}
