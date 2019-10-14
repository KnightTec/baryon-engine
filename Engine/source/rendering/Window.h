#pragma once
#include "VirtualScreen.h"
#include "../WindowsApplication.h"

#include <DirectXMath.h>

#include "windows.h"

namespace Baryon
{
class Window
{
	friend class Renderer;
public:
	Window(DirectX::XMUINT2 resolution);
	HWND getHwnd() const;
	void setActiveCamera(Camera* camera);
	void show();
	virtual void setResolution(DirectX::XMUINT2 resolution) = 0;
	DirectX::XMUINT2 getResolution() const;
protected:
	HWND hwnd;
	VirtualScreen* screen;
	DirectX::XMUINT2 resolution;
};


inline Window::Window(DirectX::XMUINT2 resolution) 
	: hwnd{WindowsApplication::getWindowHandle()}, screen{nullptr}, resolution{resolution}
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
inline void Window::setResolution(DirectX::XMUINT2 resolution)
{
	this->resolution = resolution;
	screen->resize(resolution);
}
inline DirectX::XMUINT2 Window::getResolution() const
{
	return resolution;
}

}
