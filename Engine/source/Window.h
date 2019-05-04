#pragma once
#include "windows.h"

namespace Baryon
{
class Window
{
public:
	Window();
	HWND getHwnd() const;
protected:
	HWND hwnd;
};


inline Window::Window() : hwnd{nullptr}
{
}
inline HWND Window::getHwnd() const
{
	return hwnd;
}
}
