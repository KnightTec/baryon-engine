#include "VirtualScreen.h"
#include "Window.h"
#include "DXErr.h"

#include "dxgi.h"
#include <string>

using namespace Baryon;
using namespace Microsoft::WRL;

static DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

static std::vector<DirectX::XMUINT2> getSupportedResolutions()
{
	ComPtr<IDXGIFactory> factory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));

	ComPtr<IDXGIAdapter> adapter;
	factory->EnumAdapters(0, adapter.GetAddressOf());

	ComPtr<IDXGIOutput> output;
	adapter->EnumOutputs(0, output.GetAddressOf());

	UINT numModes = 0;
	DXGI_MODE_DESC* displayModes = nullptr;
	DXGI_FORMAT format = swapChainFormat;

	output->GetDisplayModeList(format, 0, &numModes, nullptr);
	displayModes = new DXGI_MODE_DESC[numModes];
	output->GetDisplayModeList(format, 0, &numModes, displayModes);

	std::vector<DirectX::XMUINT2> resolutions;
	resolutions.emplace_back(DirectX::XMUINT2{ displayModes[0].Width, displayModes[0].Height });
	for (UINT i = 1; i < numModes; i++)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::string s = "Resolution: ";
		s += std::to_string(displayModes[i].Width);
		s += "x";
		s += std::to_string(displayModes[i].Height);
		s += ", Refresh rate: ";
		s += std::to_string(displayModes[i].RefreshRate.Numerator);
		s += '/';
		s += std::to_string(displayModes[i].RefreshRate.Denominator);
		OutputDebugStringA(s.c_str());
		OutputDebugStringA("\n");
#endif
		if (displayModes[i].Width != displayModes[i-1].Width && displayModes[i].Height != displayModes[i - 1].Height)
		{
			resolutions.emplace_back(DirectX::XMUINT2{ displayModes[i].Width, displayModes[i].Height });
		}

	}
	delete[] displayModes;
	return resolutions;

}
const std::vector<DirectX::XMUINT2> VirtualScreen::supportedResolutions = getSupportedResolutions();


VirtualScreen::VirtualScreen() : initialized{ false }, fullscreen{ false }, activeCamera{ nullptr }, resolution{ 0, 0 }
{
}

bool VirtualScreen::initialize(const Window& window)
{
	assert(!initialized);
	initialized = true;

	// Create the swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swapChainDesc.Width = supportedResolutions[0].x;
	swapChainDesc.Height = supportedResolutions[0].y;
	swapChainDesc.Format = swapChainFormat;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGIDevice4> dxgiDevice;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIFactory2> factory;
	ComPtr<IDXGISwapChain1> swapChain;
	HR(getDevice()->QueryInterface(__uuidof(IDXGIDevice4), static_cast<void**>(&dxgiDevice)));
	HR(dxgiDevice->GetAdapter(adapter.GetAddressOf()));
	HR(adapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(factory.GetAddressOf())));

	HR(factory->CreateSwapChainForHwnd(getDevice(), window.getHwnd(), &swapChainDesc, nullptr, nullptr, &swapChain));
	HR(factory->MakeWindowAssociation(window.getHwnd(), DXGI_MWA_NO_ALT_ENTER));
	HR(swapChain.As(&d3dSwapChain));

	configureBuffers();

	return true;
}

void VirtualScreen::terminate()
{
	d3dSwapChain->SetFullscreenState(false, nullptr);
	releaseBuffers();
}

bool VirtualScreen::configureBuffers()
{
	assert(initialized);
	// Create the render target view
	HR(d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D1), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.Texture2D.MipSlice = 0;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	HR(getDevice()->CreateRenderTargetView(backBuffer.Get(), &desc, renderTargetView.GetAddressOf()));

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

	HR(getDevice()->CreateTexture2D1(&depthStencilDesc, nullptr, &depthStencilBuffer));
	HR(getDevice()->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &depthStencilView));

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(resolution.x);
	vp.Height = static_cast<float>(resolution.y);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	getContext()->RSSetViewports(1, &vp);

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
	getContext()->Flush();
}

bool VirtualScreen::resize(DirectX::XMUINT2 resolution)
{
	assert(initialized);
	this->resolution = resolution;
	DXGI_MODE_DESC mode = {};
	mode.Width = resolution.x;
	mode.Height = resolution.y;
	mode.Format = swapChainFormat;
	if (fullscreen)
	{
		HR(d3dSwapChain->ResizeTarget(&mode));
	}
	if (activeCamera)
	{
		activeCamera->setAspectRatio(getAspectRatio());
	}
	releaseBuffers();
	HR(d3dSwapChain->ResizeBuffers(0, resolution.x, resolution.y, swapChainFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	configureBuffers();
	return true;
}

bool VirtualScreen::setFullscreen(bool fullscreen)
{
	HR(d3dSwapChain->SetFullscreenState(fullscreen, nullptr));
	releaseBuffers();
	if (d3dSwapChain)
	{
		HR(d3dSwapChain->ResizeBuffers(0, resolution.x, resolution.y, swapChainFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	}
	configureBuffers();
	this->fullscreen = fullscreen;
	return true;
}
