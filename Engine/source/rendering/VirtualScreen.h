#pragma once
#include "../Camera.h"
#include "GraphicsDeviceInterface.h"
#include "RenderTexture.h"
#include "SwapChain.h"
#include "DXErr.h"

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
	bool resize(Size2D resolution);
	void present();
	void clear();
	bool setFullscreen(bool fullscreen);
	void setActiveCamera(Camera* camera);
	void setupGeometryPass();
	void setupLightPass();
	void setupPostProcessPass();
	float getAspectRatio() const;
	Camera* getActiveCamera() const;
	void setViewportSize(int width, int height);
	void setupIntermediateViewport();
	void setupFinalViewport();

	
	RenderTexture gBufferTexture1{DXGI_FORMAT_R16G16B16A16_UNORM};	// RGB color, A specular intensity 
	RenderTexture worldNormals{ DXGI_FORMAT_R32G32B32A32_FLOAT };	// world space normals	
	RenderTexture litScene{DXGI_FORMAT_R16G16B16A16_UNORM};


	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthBufferSRV;
private:
	bool configureBuffers();
	void releaseBuffers();

	bool initialized;
	Camera* activeCamera;
	D3D11_VIEWPORT viewport;
	SwapChain* swapChain;

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
inline void VirtualScreen::setActiveCamera(Camera* camera)
{
	activeCamera = camera;
	activeCamera->setAspectRatio(getAspectRatio());
}
inline float VirtualScreen::getAspectRatio() const
{
	Size2D resolution = swapChain->getBackBufferResolution();
	return static_cast<float>(resolution.x) / static_cast<float>(resolution.y);
}
inline Camera* VirtualScreen::getActiveCamera() const
{
	return activeCamera;
}
inline void VirtualScreen::clear()
{
	static const float clearColor[] = {0.f, 0.f, 0.f, 1.f};
	getContext()->ClearRenderTargetView(swapChain->getRenderTargetView(), clearColor);
	getContext()->ClearRenderTargetView(worldNormals.getRenderTargetView(), clearColor);
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
}
