#pragma once
#include "windows.h"

namespace Baryon
{
class Window
{
public:
	Window();
	virtual ~Window() = 0;
	HWND getHwnd() const;
protected:
	//TODO: add virtual screen ptr
	HWND hwnd;
};


inline Window::Window() : hwnd{nullptr}
{
}

inline Window::~Window()
{
}

inline HWND Window::getHwnd() const
{
	return hwnd;
}
}
