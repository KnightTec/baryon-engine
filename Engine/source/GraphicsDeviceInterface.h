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
protected:
	static ID3D11Device4& getDevice();
	static ID3D11DeviceContext4& getContext();
private:
	static bool initialized;
	static Microsoft::WRL::ComPtr<ID3D11Device4> d3dDevice;
	static Microsoft::WRL::ComPtr<ID3D11DeviceContext4> d3dContext;
};

inline ID3D11Device4& GraphicsDeviceInterface::getDevice()
{
	assert(initialized);
	return *d3dDevice.Get();
}
inline ID3D11DeviceContext4& GraphicsDeviceInterface::getContext()
{
	assert(initialized);
	return *d3dContext.Get();
}
}
