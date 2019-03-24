#pragma once
#include "Window.h"

#include "wrl/client.h"
#include "d3d11_4.h"

namespace Baryon
{
class RenderingEngine;

class Viewport
{
	friend class RenderingEngine;
public:

private:
	Viewport(const Window& window);
	bool init(ID3D11Device4& d3dDevice);

	HWND hwnd;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> d3dSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};
}
