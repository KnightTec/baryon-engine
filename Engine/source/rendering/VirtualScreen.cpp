#include "VirtualScreen.h"
#include "Window.h"
#include "DXErr.h"

#include "dxgi.h"
#include <string>


using namespace Baryon;
using namespace Microsoft::WRL;
using namespace DirectX;

static const DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
static DXGI_RATIONAL refreshRate = {0, 1};

static std::vector<Size2D> retrieveSupportedResolutions()
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

	std::vector<Size2D> resolutions;
	resolutions.emplace_back(Size2D{static_cast<int>(displayModes[0].Width), static_cast<int>(displayModes[0].Height)});
	for (UINT i = 1; i < numModes; i++)
	{
		// skip duplicates
		if (displayModes[i].Width != displayModes[i - 1].Width && displayModes[i].Height != displayModes[i - 1].Height)
		{
			resolutions.emplace_back(Size2D{static_cast<int>(displayModes[i].Width), static_cast<int>(displayModes[i].Height)});
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

const std::vector<Size2D>& VirtualScreen::getSupportedResolutions()
{
	static const std::vector<Size2D> supportedResolutions = retrieveSupportedResolutions();
	return supportedResolutions;
}

VirtualScreen::VirtualScreen() : initialized{false}, activeCamera{nullptr}, taaJitterCounter(0)
{
	// call to initialize the refresh rate
	getSupportedResolutions();
}

bool VirtualScreen::initialize(const Window& window)
{
	assert(!initialized);
	initialized = true;
	swapChain = new SwapChain(swapChainFormat, window.getHwnd());
	configureBuffers();
	return true;
}

void VirtualScreen::terminate()
{
	releaseBuffers();
	delete swapChain;
	initialized = false;
}



void VirtualScreen::tick()
{
	static const XMFLOAT2 rgssOffsets[] = {
		{0.75, -0.25}, {-0.25, -0.75},
		{-0.75, 0.25}, {0.25, 0.75}
	};

	XMFLOAT2 jitter = {
		rgssOffsets[taaJitterCounter].x / float(resolution.x),
		rgssOffsets[taaJitterCounter].y / float(resolution.y)
	};
	XMStoreFloat4x3(&taaJitterMatrix, XMMatrixTranslation(jitter.x, jitter.y, 0));
	//XMStoreFloat4x3(&taaJitterMatrix, XMMatrixTranslation(0, 0, 0));
	if (taaJitterCounter % 2)
	{
		historyFrame = &frameBuffer0;
		currentFrame = &frameBuffer1;
		blendedFrame = &frameBuffer2;
	}
	else
	{
		historyFrame = &frameBuffer2;
		currentFrame = &frameBuffer1;
		blendedFrame = &frameBuffer0;
	}
	taaJitterCounter = (taaJitterCounter + 1) % 4;
}

bool VirtualScreen::configureBuffers()
{
	assert(initialized);
	swapChain->setup();
	resolution = swapChain->getBackBufferResolution();

	// Create depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC1 depthStencilDesc;
	depthStencilDesc.Width = resolution.x;
	depthStencilDesc.Height = resolution.y;
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

	gBufferTexture0.create(resolution);
	gBufferTexture1.create(resolution);
	frameBuffer0.create(resolution);
	frameBuffer1.create(resolution);
	frameBuffer2.create(resolution);

	return true;
}

void VirtualScreen::releaseBuffers()
{
	assert(initialized);

	frameBuffer0.release();
	frameBuffer1.release();
	frameBuffer2.release();
	gBufferTexture0.release();
	gBufferTexture1.release();
	depthBufferSRV.Reset();
	depthStencilView.Reset();
	depthStencilBuffer.Reset();
	swapChain->reset();

	getContext()->Flush();
}

bool VirtualScreen::resize(const Size2D& resolution)
{
	assert(initialized);
	releaseBuffers();
	swapChain->resize(resolution);
	configureBuffers();
	if (activeCamera)
	{
		auto* cam = activeCamera->getComponent<CameraComponent>();
		if (cam)
		{
			cam->aspectRatio = getAspectRatio();
		}
	}
	return true;
}
bool VirtualScreen::setFullscreen(bool fullscreen)
{
	releaseBuffers();
	swapChain->setFullscreen(fullscreen);
	configureBuffers();
	return true;
}
void VirtualScreen::setActiveCamera(Entity* entity)
{
	activeCamera = entity;
	auto* cam = entity->getComponent<CameraComponent>();
	if (cam)
	{
		cam->aspectRatio = getAspectRatio();
	}
}
CameraComponent* VirtualScreen::getActiveCamera()
{
	if (activeCamera == nullptr)
	{
		return nullptr;
	}
	return activeCamera->getComponent<CameraComponent>();
}
void VirtualScreen::setupGeometryPass()
{
	ID3D11RenderTargetView* rtvs[] = {gBufferTexture0.getRenderTargetView(), gBufferTexture1.getRenderTargetView()};
	getContext()->OMSetRenderTargets(2, rtvs, depthStencilView.Get());
}
void VirtualScreen::setupLightPass()
{
	ID3D11RenderTargetView* rtvs[] = {currentFrame->getRenderTargetView(), nullptr, nullptr};
	getContext()->OMSetRenderTargets(3, rtvs, nullptr);
	ID3D11ShaderResourceView* srvs[] = {
		gBufferTexture0.getShaderResourceView(),
		gBufferTexture1.getShaderResourceView(),
		depthBufferSRV.Get()
	};
	getContext()->PSSetShaderResources(0, 3, srvs);
}
void VirtualScreen::setupAAPass()
{
	ID3D11RenderTargetView* rtvs[] = {blendedFrame->getRenderTargetView(), nullptr, nullptr};
	getContext()->OMSetRenderTargets(3, rtvs, nullptr);
	ID3D11ShaderResourceView* srvs2[] = {historyFrame->getShaderResourceView(), currentFrame->getShaderResourceView()};
	getContext()->PSSetShaderResources(4, 2, srvs2);
}
void VirtualScreen::setupPostProcessPass()
{
	ID3D11RenderTargetView* rtvs[] = {swapChain->getRenderTargetView()};
	getContext()->OMSetRenderTargets(1, rtvs, nullptr);
	ID3D11ShaderResourceView* srvs2[] = {blendedFrame->getShaderResourceView()};
	getContext()->PSSetShaderResources(3, 1, srvs2);
}
void VirtualScreen::setViewportSize(int width, int height)
{
	viewport.Width = width;
	viewport.Height = height;
}
