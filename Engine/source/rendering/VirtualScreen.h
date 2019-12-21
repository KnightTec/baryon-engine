#pragma once
#include "GraphicsDeviceInterface.h"
#include "RenderTexture.h"
#include "SwapChain.h"
#include "DXErr.h"
#include "components/CameraComponent.h"
#include "Entity.h"

#include "wrl/client.h"
#include "d3d11_4.h"

#include <vector>


namespace Baryon
{
class Window;

/*
 * A VirtualScreen represents a surface within a window that displays the current front buffer
 */
class VirtualScreen : GraphicsDeviceInterface
{
public:
	static const std::vector<Size2D>& getSupportedResolutions();

	VirtualScreen();

	bool initialize(const Window& window);
	void terminate();
	void tick();
	bool resize(const Size2D& resolution);
	void present();
	void clear();
	bool setFullscreen(bool fullscreen);

	void setActiveCamera(Entity* entity);
	CameraComponent* getActiveCamera();

	void setupGeometryPass();
	void setupLightPass();
	void setupAAPass();
	void setupPostProcessPass();
	float getAspectRatio() const;

	void setViewportSize(int width, int height);
	void setupIntermediateViewport();
	void setupFinalViewport();
	const Size2D& getResolution() const;

	DirectX::XMMATRIX getTaaJitterMatrix() const;
private:
	bool configureBuffers();
	void releaseBuffers();

	bool initialized;

	Entity* activeCamera = nullptr;
	D3D11_VIEWPORT viewport;
	SwapChain* swapChain;
	Size2D resolution;

	int taaJitterCounter;
	DirectX::XMFLOAT4X3 taaJitterMatrix;

	RenderTexture frameBuffer0{DXGI_FORMAT_R16G16B16A16_FLOAT};
	RenderTexture frameBuffer1{DXGI_FORMAT_R16G16B16A16_FLOAT};
	RenderTexture frameBuffer2{DXGI_FORMAT_R16G16B16A16_FLOAT};

	RenderTexture* historyFrame;
	RenderTexture* currentFrame;
	RenderTexture* blendedFrame;

	// G-Buffer
	RenderTexture gBufferTexture0{DXGI_FORMAT_R8G8B8A8_UNORM}; // RGB color, A specular intensity 
	RenderTexture gBufferTexture1{DXGI_FORMAT_R32G32B32A32_FLOAT}; // world space normals
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthBufferSRV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D1> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementations 
///////////////////////////////////////////////////////////////////////////////////////////////
inline void VirtualScreen::present()
{
	swapChain->present();
}
inline float VirtualScreen::getAspectRatio() const
{
	Size2D resolution = swapChain->getBackBufferResolution();
	return static_cast<float>(resolution.x) / static_cast<float>(resolution.y);
}
inline void VirtualScreen::clear()
{
	static const float clearColor[] = {0.f, 0.f, 0.f, 1.f};
	getContext()->ClearRenderTargetView(swapChain->getRenderTargetView(), clearColor);
	getContext()->ClearRenderTargetView(gBufferTexture0.getRenderTargetView(), clearColor);
	getContext()->ClearRenderTargetView(gBufferTexture1.getRenderTargetView(), clearColor);
	getContext()->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
}
inline void VirtualScreen::setupIntermediateViewport()
{
	D3D11_VIEWPORT vp = viewport;
	Size2D resolution = swapChain->getBackBufferResolution();
	vp.Width = resolution.x;
	vp.Height = resolution.y;
	getContext()->RSSetViewports(1, &vp);
}
inline void VirtualScreen::setupFinalViewport()
{
	getContext()->RSSetViewports(1, &viewport);
}
inline const Size2D& VirtualScreen::getResolution() const
{
	return resolution;
}
inline DirectX::XMMATRIX VirtualScreen::getTaaJitterMatrix() const
{
	return DirectX::XMLoadFloat4x3(&taaJitterMatrix);
}
}
