#include "VirtualScreen.h"
#include "Window.h"
#include "DXErr.h"

#include "dxgi.h"
#include <string>
#include <minwinbase.h>


using namespace Baryon;
using namespace Microsoft::WRL;

static const DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static DXGI_RATIONAL refreshRate = {0, 1};

static std::vector<DirectX::XMUINT2> retrieveSupportedResolutions()
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
	resolutions.emplace_back(DirectX::XMUINT2{displayModes[0].Width, displayModes[0].Height});
	for (UINT i = 1; i < numModes; i++)
	{
		// skip duplicates
		if (displayModes[i].Width != displayModes[i - 1].Width && displayModes[i].Height != displayModes[i - 1].Height)
		{
			resolutions.emplace_back(DirectX::XMUINT2{displayModes[i].Width, displayModes[i].Height});
		}
		// get the maximal refresh rate
		DXGI_RATIONAL refreshRateTmp = displayModes[i].RefreshRate;
		if (refreshRateTmp.Numerator / refreshRateTmp.Denominator > refreshRate.Numerator / refreshRate.Denominator)
		{
			refreshRate = refreshRateTmp;
		}
	}
	delete[] displayModes;
	return resolutions;
}

const std::vector<DirectX::XMUINT2>& VirtualScreen::getSupportedResolutions()
{
	static const std::vector<DirectX::XMUINT2> supportedResolutions = retrieveSupportedResolutions();
	return supportedResolutions;
}

VirtualScreen::VirtualScreen() : initialized{false}, fullscreen{false}, activeCamera{nullptr}, resolution{0, 0}
{
	// call to initialize the refresh rate
	getSupportedResolutions();
}

bool VirtualScreen::initialize(const Window& window)
{
	assert(!initialized);
	initialized = true;
	
	// Create the swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swapChainDesc.Width = getSupportedResolutions()[0].x;
	swapChainDesc.Height = getSupportedResolutions()[0].y;
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
	initialized = false;
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
	depthStencilDesc.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.TextureLayout = D3D11_TEXTURE_LAYOUT_UNDEFINED;
	HR(getDevice()->CreateTexture2D1(&depthStencilDesc, nullptr, &depthStencilBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(getDevice()->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, &depthStencilView));

	D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc;
	depthSrvDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	depthSrvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2D;
	depthSrvDesc.Texture2D.MipLevels = depthStencilDesc.MipLevels;
	depthSrvDesc.Texture2D.MostDetailedMip = 0;
	HR(getDevice()->CreateShaderResourceView(depthStencilBuffer.Get(), &depthSrvDesc, &depthBufferSRV));

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(resolution.x);
	viewport.Height = static_cast<float>(resolution.y);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	worldNormals.create({swapChainDesc.Width, swapChainDesc.Height});
	litScene.create({ swapChainDesc.Width, swapChainDesc.Height });

	return true;
}

void VirtualScreen::releaseBuffers()
{
	assert(initialized);
	renderTargetView.Reset();
	backBuffer.Reset();
	depthStencilView.Reset();
	depthStencilBuffer.Reset();

	litScene.release();
	worldNormals.release();

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
	mode.RefreshRate = refreshRate;
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
	if (fullscreen)
	{
		DXGI_MODE_DESC mode = {};
		mode.Width = resolution.x;
		mode.Height = resolution.y;
		mode.Format = swapChainFormat;
		HR(d3dSwapChain->ResizeTarget(&mode));
	}
	releaseBuffers();
	if (d3dSwapChain)
	{
		HR(d3dSwapChain->ResizeBuffers(0, resolution.x, resolution.y, swapChainFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	}
	configureBuffers();
	this->fullscreen = fullscreen;
	return true;
}

void VirtualScreen::setupGeometryPass()
{
	ID3D11RenderTargetView* rtvs[] = {worldNormals.getRenderTargetView()};  
	getContext()->OMSetRenderTargets(1, rtvs, depthStencilView.Get());
}

void VirtualScreen::setupLightPass()
{
	ID3D11RenderTargetView* rtvs[] = { litScene.getRenderTargetView() };
	getContext()->OMSetRenderTargets(1, rtvs, nullptr);
}
void VirtualScreen::setupPostProcessPass()
{
	getContext()->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);
}
void VirtualScreen::setViewportSize(int width, int height)
{
	viewport.Width = width;
	viewport.Height = height;
}

