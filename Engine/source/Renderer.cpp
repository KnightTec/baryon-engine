#include "Renderer.h"
#include "VirtualScreen.h"
#include "Mesh.h"
#include "RenderPass.h"
#include "DXErr.h"

using namespace Baryon;
using namespace DirectX;
using namespace Microsoft::WRL;


struct VS_CONSTANT_BUFFER
{
	XMFLOAT4X4 mWorldViewProj;
	XMFLOAT4X4 mWorldViewProjInv;
};

ComPtr<ID3D11RasterizerState1> rasterState;

static VertexShader vs{L"../../Engine/shaders/VertexShader.hlsl"};
static PixelShader ps{L"../../Engine/shaders/PixelShader.hlsl"};

bool Renderer::initialize()
{
	vs.compile();
	ps.compile();

	vs.apply();
	ps.apply();

	getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create rasterizer state
	D3D11_RASTERIZER_DESC1 rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	rasterizerState.ForcedSampleCount = 0;
	HR(getDevice()->CreateRasterizerState1(&rasterizerState, rasterState.GetAddressOf()));
	getContext()->RSSetState(rasterState.Get());

	/*
	 * TODO: Blend State
	  Depth Stencil State
	  Sampler State
	 */
	return true;
}

void Renderer::terminate()
{
	for (VirtualScreen& screen : virtualScreens)
	{
		screen.terminate();
	}
}

bool Renderer::createVirtualScreen(Window& targetWindow)
{
	virtualScreens.emplace_back(VirtualScreen{});
	if (!virtualScreens.back().initialize(targetWindow))
	{
		virtualScreens.pop_back();
		return false;
	}
	targetWindow.screen = &virtualScreens.back();
	targetWindow.initialize();
	return true;
}

void Renderer::render()
{
	for (VirtualScreen& screen : virtualScreens)
	{
		screen.clear();
	}
	for (const Mesh* mesh : meshes)
	{
		uint32_t strides = sizeof(Vertex);
		uint32_t offsets = 0;
		ID3D11Buffer* vertexBuffer = mesh->getVertexBuffer();
		getContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		getContext()->IASetIndexBuffer(mesh->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		for (VirtualScreen& screen : virtualScreens)
		{
			Camera* cam = screen.getActiveCamera();
			if (cam)
			{
				ID3D11RenderTargetView* rtv = screen.getRenderTargetView();
				getContext()->OMSetRenderTargets(1, &rtv, screen.getDepthStencilView());

				// update constant buffer (matrices)
				VS_CONSTANT_BUFFER data;
				XMStoreFloat4x4(&data.mWorldViewProj, XMMatrixTranspose(cam->getViewProjMatrix()));
				XMStoreFloat4x4(&data.mWorldViewProjInv, XMMatrixInverse(nullptr, cam->getViewProjMatrix()));
				vs.updateConstantBufferByIndex(&data, sizeof(data), 0);

				getContext()->DrawIndexed(mesh->getIndexCount(), 0, 0);
			}
		}
	}
	for (VirtualScreen& screen : virtualScreens)
	{
		screen.present();
	}
}
