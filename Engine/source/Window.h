#pragma once
#include "windows.h"
#include "WindowsApplication.h"
#include "VirtualScreen.h"


namespace Baryon
{
class Window
{
	friend class Renderer;
public:
	Window();
	HWND getHwnd() const;
	void setActiveCamera(Camera* camera);
	void show();

	void setVirtualScreen(VirtualScreen* vs);
protected:
	virtual bool initialize() = 0;

	HWND hwnd;
	VirtualScreen* screen;
};


inline Window::Window() : hwnd{WindowsApplication::createEmptyWindow()}, screen{nullptr}
{
}

inline HWND Window::getHwnd() const
{
	return hwnd;
}

inline void Window::setActiveCamera(Camera* camera)
{
	screen->setActiveCamera(camera);
}

inline void Window::show()
{
	ShowWindow(hwnd, SW_SHOW);
}
}
