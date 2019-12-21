#pragma once
#include "VirtualScreen.h"
#include "Size.h"

#include "windows.h"

namespace Baryon
{
class Window
{
	friend class Renderer;
	friend class RenderingEngine;
public:
	Window(Size2D resolution);
	HWND getHwnd() const;
	void setActiveCamera(Entity* entity);
	Size2D getResolution() const;
	virtual void setResolution(Size2D resolution);
protected:
	HWND hwnd;
	VirtualScreen* screen;
	Size2D resolution;
};


inline Window::Window(Size2D resolution) 
	: hwnd{nullptr}, screen{nullptr}, resolution{resolution}
{
}
inline HWND Window::getHwnd() const
{
	return hwnd;
}
inline void Window::setActiveCamera(Entity* entity)
{
	if (screen)
	{
		screen->setActiveCamera(entity);
	}
}

inline void Window::setResolution(Size2D resolution)
{
	this->resolution = resolution;
	if (screen) {
		screen->resize(resolution);
	}
}
inline Size2D Window::getResolution() const
{
	return resolution;
}

}
