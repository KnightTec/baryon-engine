#include "GraphicsDeviceInterface.h"

#include "wrl/client.h"
#include <cassert>


using namespace Baryon;
using namespace Microsoft::WRL;

bool GraphicsDeviceInterface::initialized = false;
ID3D11Device4* GraphicsDeviceInterface::d3dDevice;
ID3D11DeviceContext4* GraphicsDeviceInterface::d3dContext;

bool GraphicsDeviceInterface::initialize()
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
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, levels, ARRAYSIZE(levels),
		D3D11_SDK_VERSION, &device, nullptr, &context)))
	{
		MessageBoxW(nullptr, L"Error: Failed to create device.", L"DirectX", MB_OK | MB_ICONERROR);
		return false;
	}
	if (device->GetFeatureLevel() != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBoxW(nullptr, L"Error: Device does not support feature level 11_1.", L"DirectX", MB_OK | MB_ICONERROR);
		return false;
	}
	device->QueryInterface(__uuidof(d3dDevice), reinterpret_cast<void**>(&d3dDevice));
	context->QueryInterface(__uuidof(d3dContext), reinterpret_cast<void**>(&d3dContext));

	device->Release();
	context->Release();

	initialized = true;
	return true;
}
void GraphicsDeviceInterface::terminate()
{
#ifdef _DEBUG
	ID3D11Debug* debug;
	d3dDevice->QueryInterface(IID_PPV_ARGS(&debug));
#endif
	d3dContext->Release();
	d3dDevice->Release();
#ifdef _DEBUG
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	debug->Release();
#endif

}