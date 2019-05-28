#pragma once
#include "PassKey.h"

#include "wrl/client.h"
#include "d3d11_4.h"
#include <cassert>

namespace Baryon
{
class Engine;

/*
 * A service locator that provides a interface to the graphics API
 * All classes that wish to use the graphics API must inherit from this class
 */
class GraphicsDeviceInterface
{
public:
	static bool initialize(Key<Engine>);
	static void terminate(Key<Engine>);
protected:
	GraphicsDeviceInterface();
	static ID3D11Device4* getDevice();
	static ID3D11DeviceContext4* getContext();
private:
	static ID3D11Device4** getDevicePtr();
	static ID3D11DeviceContext4** getContextPtr();
	static ID3D11Device4* d3device;
	static ID3D11DeviceContext4* d3dContext;
	static bool initialized;
};

inline ID3D11Device4* GraphicsDeviceInterface::getDevice()
{
	return *getDevicePtr();
}
inline ID3D11DeviceContext4* GraphicsDeviceInterface::getContext()
{
	return *getContextPtr();
}
inline ID3D11Device4** GraphicsDeviceInterface::getDevicePtr()
{
	static Microsoft::WRL::ComPtr<ID3D11Device4> d3dDevice;
	return d3dDevice.GetAddressOf();
}
inline ID3D11DeviceContext4** GraphicsDeviceInterface::getContextPtr()
{
	static Microsoft::WRL::ComPtr<ID3D11DeviceContext4> d3dContext;
	return d3dContext.GetAddressOf();
}
}
