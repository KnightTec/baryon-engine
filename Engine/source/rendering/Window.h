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
	DirectX::XMUINT2 getResolution() const;

	virtual void setResolution(DirectX::XMUINT2 resolution);
protected:
	HWND hwnd;
	VirtualScreen* screen;
	DirectX::XMUINT2 resolution;
};


inline Window::Window(DirectX::XMUINT2 resolution) 
	: hwnd{nullptr}, screen{nullptr}, resolution{resolution}
{
}
inline HWND Window::getHwnd() const
{
	return hwnd;
}
inline void Window::setActiveCamera(Camera* camera)
{
	if (screen) {
		screen->setActiveCamera(camera);
	}
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
