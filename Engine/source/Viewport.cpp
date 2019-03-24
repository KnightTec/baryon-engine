#include "Viewport.h"

using namespace Baryon;
using namespace Microsoft::WRL;

Viewport::Viewport(const Window& window) : hwnd{window.getHwnd()}
{
}
bool Viewport::init(ID3D11Device4& d3dDevice)
{
	//TODO: Check MSAA support
	//TODO: Handle errors

	// Create the swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGIDevice4> dxgiDevice;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIFactory2> factory;
	ComPtr<IDXGISwapChain1> swapChain;
	d3dDevice.QueryInterface(__uuidof(IDXGIDevice4), static_cast<void**>(&dxgiDevice));
	dxgiDevice->GetAdapter(adapter.GetAddressOf());
	adapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(factory.GetAddressOf()));

	factory->CreateSwapChainForHwnd(&d3dDevice, hwnd, &swapChainDesc, nullptr, nullptr, &swapChain);
	factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	swapChain.As(&d3dSwapChain);

	swapChain->GetDesc1(&swapChainDesc);

	// Create the render target view
	ComPtr<ID3D11Texture2D> backBuffer;
	d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.Texture2D.MipSlice = 0;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	d3dDevice.CreateRenderTargetView(backBuffer.Get(), &desc, renderTargetView.GetAddressOf());

	// Create depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC1 depthStencilDesc;
	depthStencilDesc.Width = swapChainDesc.Width;
	depthStencilDesc.Height = swapChainDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.TextureLayout = D3D11_TEXTURE_LAYOUT_UNDEFINED;

	ComPtr<ID3D11Texture2D1> depthStencilBuffer;
	d3dDevice.CreateTexture2D1(&depthStencilDesc, 0, &depthStencilBuffer);
	d3dDevice.CreateDepthStencilView(depthStencilBuffer.Get(), 0, &depthStencilView);

	return true;
}
