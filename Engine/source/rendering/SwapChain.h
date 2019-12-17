#pragma once
#include "GraphicsDeviceInterface.h"
#include "Size.h"

#include "wrl/client.h"
#include "d3d11_4.h"
#include "DXErr.h"


namespace Baryon
{
class SwapChain : GraphicsDeviceInterface
{
public:
	SwapChain(DXGI_FORMAT format, HWND hwnd);
	void setup();
	void reset();
	void present();
	void resize(int width, int height);
	void resize(Size2D resolution);
	void setFullscreen(bool fullscreen);
	Size2D getBackBufferResolution() const;
	ID3D11RenderTargetView* getRenderTargetView() const;
private:
	bool isFullscreen;
	DXGI_FORMAT format;
	Size2D backBufferResolution;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> d3dSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D1> backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
};


inline void SwapChain::present()
{
	HRF(d3dSwapChain->Present(1, 0));
}
inline void SwapChain::resize(Size2D resolution)
{
	resize(resolution.x, resolution.y);
}
inline Size2D SwapChain::getBackBufferResolution() const
{
	return backBufferResolution;
}
inline ID3D11RenderTargetView* SwapChain::getRenderTargetView() const
{
	return renderTargetView.Get();
}
}
