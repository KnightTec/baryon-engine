#pragma once
#include "d3d11_4.h"

namespace Baryon
{
class Engine;

/*
 * A service locator that provides a interface to the graphics API
 * All classes that use the graphics API must inherit from this class
 */
class GraphicsDeviceInterface
{
public:
	static bool initialize();
	static void terminate();
protected:
	static ID3D11Device4* getDevice();
	static ID3D11DeviceContext4* getContext();
private:
	static bool initialized;
	static ID3D11Device4* d3dDevice;
	static ID3D11DeviceContext4* d3dContext;
};


inline ID3D11Device4* GraphicsDeviceInterface::getDevice()
{
	return d3dDevice;
}
inline ID3D11DeviceContext4* GraphicsDeviceInterface::getContext()
{
	return d3dContext;
}
}
