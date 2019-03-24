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
		WINDOWED_SCALABLE = 2	//TODO: remove this style
	};

	Window(DirectX::XMUINT2 clientSize, STYLE style);

	HWND getHwnd() const;
	DirectX::XMUINT2 getClientSize() const;
	void setStyle(STYLE newStyle);
	void resize(DirectX::XMUINT2 newClientSize);
private:
	HWND windowHandle;
};

inline HWND Window::getHwnd() const
{
	return windowHandle;
}

}
