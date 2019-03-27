#include "GraphicsDeviceInterface.h"

using namespace Baryon;
using namespace Microsoft::WRL;

bool GraphicsDeviceInterface::initialized = false;
ComPtr<ID3D11Device4> GraphicsDeviceInterface::d3dDevice;
ComPtr<ID3D11DeviceContext4> GraphicsDeviceInterface::d3dContext;

bool GraphicsDeviceInterface::initialize(Key<Engine>)
{
	assert(!initialized);
	UINT flags = 0;
#if defined(_DEBUG) || defined(DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, levels, ARRAYSIZE(levels),
		D3D11_SDK_VERSION, device.GetAddressOf(), nullptr, context.GetAddressOf())))
	{
		MessageBoxW(nullptr, L"Error: Failed to create device.", L"DirectX", MB_OK | MB_ICONERROR);
		return false;
	}
	if (device->GetFeatureLevel() != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBoxW(nullptr, L"Error: Device does not support feature level 11_1.", L"DirectX", MB_OK | MB_ICONERROR);
		return false;
	}
	device.As(&d3dDevice);
	context.As(&d3dContext);
	initialized = true;
	return true;
}
