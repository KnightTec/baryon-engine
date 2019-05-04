#include "VirtualScreen.h"
#include "Window.h"
#include "DXErr.h"

using namespace Baryon;
using namespace Microsoft::WRL;


VirtualScreen::VirtualScreen() : initialized{false}, resolution{0, 0}
{
}

bool VirtualScreen::initialize(const Window& window)
{
	//TODO: Check MSAA support
	assert(!initialized);
	initialized = true;

	// Create the swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
	HR(getDevice().QueryInterface(__uuidof(IDXGIDevice4), static_cast<void**>(&dxgiDevice)));
	HR(dxgiDevice->GetAdapter(adapter.GetAddressOf()));
	HR(adapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(factory.GetAddressOf())));

	HR(factory->CreateSwapChainForHwnd(&getDevice(), window.getHwnd(), &swapChainDesc, nullptr, nullptr, &swapChain));
	HR(factory->MakeWindowAssociation(window.getHwnd(), DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES));
	HR(swapChain.As(&d3dSwapChain));

	configureBuffers();

	return true;
}

bool VirtualScreen::configureBuffers()
{
	assert(initialized);
	// Create the render target view
	HR(d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.Texture2D.MipSlice = 0;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	HR(getDevice().CreateRenderTargetView(backBuffer.Get(), &desc, renderTargetView.GetAddressOf()));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	HR(d3dSwapChain->GetDesc1(&swapChainDesc));
	resolution = {swapChainDesc.Width, swapChainDesc.Height};

	// Create depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC1 depthStencilDesc;
	depthStencilDesc.Width = swapChainDesc.Width;
	depthStencilDesc.Height = swapChainDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.TextureLayout = D3D11_TEXTURE_LAYOUT_UNDEFINED;

	HR(getDevice().CreateTexture2D1(&depthStencilDesc, nullptr, &depthStencilBuffer));
	HR(getDevice().CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &depthStencilView));

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(resolution.x);
	vp.Height = static_cast<float>(resolution.y);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	getContext().RSSetViewports(1, &vp);

	return true;
}

void VirtualScreen::releaseBuffers()
{
	assert(initialized);
	// Release the render target view based on the back buffer:
	renderTargetView.Reset();

	// Release the back buffer itself:
	backBuffer.Reset();

	// The depth stencil will need to be resized, so release it (and view):
	depthStencilView.Reset();
	depthStencilBuffer.Reset();

	// After releasing references to these resources, we need to call Flush() to 
	// ensure that Direct3D also releases any references it might still have to
	// the same resources - such as pipeline bindings.
	getContext().Flush();
}

bool VirtualScreen::resize(DirectX::XMUINT2 resolution)
{
	assert(initialized);
	this->resolution = resolution;
	if (activeCamera) {
		activeCamera->setAspectRatio(getAspectRatio());
	}
	releaseBuffers();
	HR(d3dSwapChain->ResizeBuffers(0, resolution.x, resolution.y, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	configureBuffers();
	return true;
}
