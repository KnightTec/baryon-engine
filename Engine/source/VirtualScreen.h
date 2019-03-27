#pragma once
#include "GraphicsDeviceInterface.h"

#include "wrl/client.h"
#include "d3d11_4.h"
#include "DirectXMath.h"

namespace Baryon
{
class GameWindow;

class VirtualScreen : private GraphicsDeviceInterface
{
public:
	VirtualScreen() = default;
	bool initialize(GameWindow& window);
	void resize(DirectX::XMUINT2 resolution);
	bool present();
	ID3D11RenderTargetView* getRenderTargetView() const;
	ID3D11DepthStencilView* getDepthStencilView() const;
private:
	// TODO: add camera*
	Microsoft::WRL::ComPtr<IDXGISwapChain4> d3dSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};

inline bool VirtualScreen::present()
{
	if (FAILED(d3dSwapChain->Present(1, 0)))
	{
		return false;
	}
	return true;
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
