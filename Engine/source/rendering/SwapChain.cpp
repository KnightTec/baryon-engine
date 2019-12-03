#include "SwapChain.h"
#include "DXErr.h"

using namespace Baryon;
using namespace Microsoft::WRL;


SwapChain::SwapChain(DXGI_FORMAT format, HWND hwnd) 
	: isFullscreen(false), format(format), backBufferResolution{ 800, 600 }
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swapChainDesc.Width = backBufferResolution.x;
	swapChainDesc.Height = backBufferResolution.y;
	swapChainDesc.Format = format;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGIDevice4> dxgiDevice;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIFactory2> factory;
	ComPtr<IDXGISwapChain1> swapChain;
	HRF(getDevice()->QueryInterface(__uuidof(IDXGIDevice4), static_cast<void**>(&dxgiDevice)));
	HRF(dxgiDevice->GetAdapter(adapter.GetAddressOf()));
	HRF(adapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(factory.GetAddressOf())));

	HRF(factory->CreateSwapChainForHwnd(getDevice(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain));
	HRF(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
	HRF(swapChain.As(&d3dSwapChain));
}
void SwapChain::setup()
{
	HRF(d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D1), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.Texture2D.MipSlice = 0;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	HRF(getDevice()->CreateRenderTargetView(backBuffer.Get(), &desc, renderTargetView.GetAddressOf()));
}
void SwapChain::reset()
{
//	HRF(d3dSwapChain->SetFullscreenState(false, nullptr));
	renderTargetView.Reset();
	backBuffer.Reset();
}
void SwapChain::resize(int width, int height)
{
	//reset();

	DXGI_MODE_DESC mode = {};
	mode.Width = width;
	mode.Height = height;
	mode.Format = format;
	/*mode.RefreshRate = refreshRate;*/
	mode.RefreshRate = {0, 1};

	if (isFullscreen)
	{
		HRF(d3dSwapChain->ResizeTarget(&mode));
	}
	HRF(d3dSwapChain->ResizeBuffers(0, width, height, format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	//setup();
	backBufferResolution.x = width;
	backBufferResolution.y = height;
}
void SwapChain::setFullscreen(bool fullscreen)
{
	HRF(d3dSwapChain->SetFullscreenState(fullscreen, nullptr));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	HRF(d3dSwapChain->GetDesc1(&swapChainDesc));
	if (fullscreen)
	{
		DXGI_MODE_DESC mode = {};
		mode.Width = swapChainDesc.Width;
		mode.Height = swapChainDesc.Height;
		mode.Format = format;
		HRF(d3dSwapChain->ResizeTarget(&mode));
	}
	HRF(d3dSwapChain->ResizeBuffers(0, swapChainDesc.Width, swapChainDesc.Height, format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	isFullscreen = fullscreen;
}
