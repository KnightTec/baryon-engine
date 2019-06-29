#pragma once
#include "../Camera.h"
#include "GraphicsDeviceInterface.h"

#include "wrl/client.h"
#include "d3d11_4.h"
#include "DirectXMath.h"
#include <vector>
#include "RenderTexture.h"

namespace Baryon
{
class Window;

/*
 * A VirtualScreen represents a surface within a window that displays the current front buffer
 */
class VirtualScreen : GraphicsDeviceInterface
{
public:
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

	DirectX::XMUINT2 getResolution() const;
	float getAspectRatio() const;
	Camera* getActiveCamera() const;

	static const std::vector<DirectX::XMUINT2>& getSupportedResolutions();


	RenderTexture worldNormals{DXGI_FORMAT_R32G32B32A32_FLOAT};
	RenderTexture litScene{DXGI_FORMAT_R8G8B8A8_UNORM};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthBufferSRV;
private:
	bool configureBuffers();
	void releaseBuffers();

	bool initialized;
	bool fullscreen;
	Camera* activeCamera;
	DirectX::XMUINT2 resolution;

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
	return !(FAILED(d3dSwapChain->Present(1, 0)));
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
	static const float clearColor[] = {0.01, 0.01, 0.01, 1.000f};
	getContext()->ClearRenderTargetView(renderTargetView.Get(), clearColor);
	float clearColor2[] = {0, 0, 0, 1};
	getContext()->ClearRenderTargetView(worldNormals.getRenderTargetView(), clearColor2);
	getContext()->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
}
