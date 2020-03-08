#pragma once
#include "GraphicsDeviceInterface.h"
#include "Size.h"

#include <dxgi.h>
#include "wrl/client.h"

namespace Baryon
{
class RenderTexture : GraphicsDeviceInterface
{
public:
	RenderTexture(DXGI_FORMAT format);
	bool create(Size2D resolution);
	void release();

	ID3D11ShaderResourceView* getShaderResourceView();
	ID3D11RenderTargetView* getRenderTargetView();
	ID3D11UnorderedAccessView* getUnorderedAccessView();
private:
	DXGI_FORMAT format;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;
};


inline ID3D11ShaderResourceView* RenderTexture::getShaderResourceView()
{
	return shaderResourceView.Get();
}

inline ID3D11RenderTargetView* RenderTexture::getRenderTargetView()
{
	return renderTargetView.Get();
}
inline ID3D11UnorderedAccessView* RenderTexture::getUnorderedAccessView()
{
	return unorderedAccessView.Get();
}
}
