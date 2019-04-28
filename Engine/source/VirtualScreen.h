#pragma once
#include "Camera.h"
#include "GraphicsDeviceInterface.h"

#include "wrl/client.h"
#include "d3d11_4.h"
#include "DirectXMath.h"

namespace Baryon
{
class GameWindow;

/*
 * A VirtualScreen represents a surface within a window that displays the current front buffer
 */
class VirtualScreen : private GraphicsDeviceInterface
{
public:
	VirtualScreen();

	bool initialize(GameWindow& window);
	void resize(DirectX::XMUINT2 resolution);
	bool present();

	DirectX::XMUINT2 getResolution() const;
	float getAspectRatio() const;
	Camera* getActiveCamera() const;
	ID3D11RenderTargetView* getRenderTargetView() const;
	ID3D11DepthStencilView* getDepthStencilView() const;

	void setActiveCamera(Camera* camera);
private:
	Camera* activeCamera;
	DirectX::XMUINT2 resolution;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> d3dSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementations 
///////////////////////////////////////////////////////////////////////////////////////////////
inline bool VirtualScreen::present()
{
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

inline ID3D11RenderTargetView* VirtualScreen::getRenderTargetView() const
{
	return renderTargetView.Get();
}
inline ID3D11DepthStencilView* VirtualScreen::getDepthStencilView() const
{
	return depthStencilView.Get();
}
}
