#include "RenderingEngine.h"
#include <cassert>

using namespace Baryon;
using namespace Microsoft::WRL;

bool RenderingEngine::initialized = false;

bool RenderingEngine::startUp()
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
bool RenderingEngine::createViewport(const Window& window, Viewport* outViewport)
{
	assert(initialized);
	viewports.emplace_back(Viewport{ window });
	outViewport = &viewports[viewports.size() - 1];
	return outViewport->init(*d3dDevice.Get());
}
void RenderingEngine::render()
{
	for (const Viewport& viewport : viewports)
	{
		d3dContext->OMSetRenderTargets(1, viewport.renderTargetView.GetAddressOf(), viewport.depthStencilView.Get());

		const float clearColor[] = { 0.739f, 0.339f, 0.139f, 1.000f };
		d3dContext->ClearRenderTargetView(viewport.renderTargetView.Get(), clearColor);

		viewport.d3dSwapChain->Present(1, 0);
	}
}
