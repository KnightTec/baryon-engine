#include "RenderTexture.h"
#include "DXErr.h"

using namespace Baryon;

RenderTexture::RenderTexture(DXGI_FORMAT format) : format{format}
{
}

bool RenderTexture::create(Size2D resolution)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = resolution.x;
	texDesc.Height = resolution.y;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	HR(getDevice()->CreateTexture2D(&texDesc, nullptr, texture.GetAddressOf()));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HR(getDevice()->CreateRenderTargetView(texture.Get(), &rtvDesc, renderTargetView.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(getDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, shaderResourceView.GetAddressOf()));

	return true;
}

void RenderTexture::release()
{
	renderTargetView.Reset();
	shaderResourceView.Reset();
	texture.Reset();
}
