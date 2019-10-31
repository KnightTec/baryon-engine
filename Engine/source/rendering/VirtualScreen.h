#pragma once
#include "../Camera.h"
#include "GraphicsDeviceInterface.h"
#include "RenderTexture.h"

#include "wrl/client.h"
#include "d3d11_4.h"
#include "DirectXMath.h"
#include <vector>
#include "DXErr.h"


namespace Baryon
{
class Window;

/*
 * A VirtualScreen represents a surface within a window that displays the current front buffer
 */
class VirtualScreen : GraphicsDeviceInterface
{
public:
	static const std::vector<DirectX::XMUINT2>& getSupportedResolutions();

	VirtualScreen();

	bool initialize(const Window& window);
	void terminate();
	bool resize(DirectX::XMUINT2 resolution);
	bool present();
	void clear();
	bool setFullscreen(bool fullscreen);
	void setActiveCamera(Camera* camera);
	void setupGeometryPass();
	void setupLightPass();
	void setupPostProcessPass();
	DirectX::XMUINT2 getResolution() const;
	float getAspectRatio() const;
	Camera* getActiveCamera() const;
	void setViewportSize(int width, int height);
	void setupIntermediateViewport();
	void setupFinalViewport();


	RenderTexture worldNormals{DXGI_FORMAT_R32G32B32A32_FLOAT};
	RenderTexture litScene{DXGI_FORMAT_R16G16B16A16_UNORM};

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthBufferSRV;
private:
	bool configureBuffers();
	void releaseBuffers();

	bool initialized;
	bool fullscreen;
	Camera* activeCamera;
	DirectX::XMUINT2 resolution;
	D3D11_VIEWPORT viewport;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> d3dSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D1> backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D1> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementations 
///////////////////////////////////////////////////////////////////////////////////////////////
inline bool VirtualScreen::present()
{
	// TODO: disable vsync
	//return !(FAILED(d3dSwapChain->Present(0, 0)));
	if (FAILED(d3dSwapChain->Present(0, 0)))
	{
		return false;
	}
	return true;
}
inline void VirtualScreen::setActiveCamera(Camera* camera)
{
	activeCamera = camera;
	activeCamera->setAspectRatio(getAspectRatio());
}
inline DirectX::XMUINT2 VirtualScreen::getResolution() const
{
	return resolution;
}
inline float VirtualScreen::getAspectRatio() const
{
	return static_cast<float>(resolution.x) / static_cast<float>(resolution.y);
}
inline Camera* VirtualScreen::getActiveCamera() const
{
	return activeCamera;
}
inline void VirtualScreen::clear()
{
	static const float clearColor[] = {0.01f, 0.01f, 0.01f, 1.000f};
	getContext()->ClearRenderTargetView(renderTargetView.Get(), clearColor);
	float clearColor2[] = {0, 0, 0, 1};
	getContext()->ClearRenderTargetView(worldNormals.getRenderTargetView(), clearColor2);
	getContext()->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
}
inline void VirtualScreen::setupIntermediateViewport()
{
	D3D11_VIEWPORT vp = viewport;
	vp.Width = resolution.x;
	vp.Height = resolution.y;
	getContext()->RSSetViewports(1, &vp);
}
inline void VirtualScreen::setupFinalViewport()
{
	getContext()->RSSetViewports(1, &viewport);
}
}
