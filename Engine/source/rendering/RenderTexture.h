#pragma once
#include "GraphicsDeviceInterface.h"
#include <DirectXMath.h>
#include <dxgi.h>

namespace Baryon
{

class RenderTexture : GraphicsDeviceInterface
{
public:
	RenderTexture(DXGI_FORMAT format);
	bool create(DirectX::XMUINT2 resolution);
	void release();

	ID3D11ShaderResourceView* getShaderResourceView();
	ID3D11RenderTargetView* getRenderTargetView();
private:
	DXGI_FORMAT format;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
};


inline ID3D11ShaderResourceView* RenderTexture::getShaderResourceView()
{
	return shaderResourceView.Get();
}

inline ID3D11RenderTargetView* RenderTexture::getRenderTargetView()
{
	return renderTargetView.Get();
}
}
