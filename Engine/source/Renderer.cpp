#include "Renderer.h"
#include "VirtualScreen.h"
#include "Mesh.h"
#include "RenderPass.h"

using namespace Baryon;
using namespace DirectX;
using namespace Microsoft::WRL;


struct VS_CONSTANT_BUFFER
{
	XMFLOAT4X4 mWorldViewProj;
	XMFLOAT4X4 mWorldViewProjInv;
};

ComPtr<ID3D11RasterizerState1> g_pRasterState;

static VertexShader vs{ L"../../Engine/shaders/VertexShader.hlsl" };
static PixelShader ps{ L"../../Engine/shaders/PixelShader.hlsl" };

bool Renderer::initialize()
{
	vs.compile();
	ps.compile();

	vs.apply();
	ps.apply();

	getContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	getDevice().CreateRasterizerState1(&rasterizerState, g_pRasterState.GetAddressOf());
	getContext().RSSetState(g_pRasterState.Get());

	/*
	 * TODO: Blend State
	  Depth Stencil State
	  Sampler State
	 */
	 //
	return true;
}
void Renderer::render()
{
	for (VirtualScreen* screen : virtualScreens)
	{
		if (screen) {
			const float clearColor[] = { 0, 0, 0, 1.000f };
			getContext().ClearRenderTargetView(screen->getRenderTargetView(), clearColor);
			getContext().ClearDepthStencilView(screen->getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}
	for (const Mesh* mesh : meshes) {
		UINT strides = sizeof(Vertex);
		UINT offsets = 0;
		ID3D11Buffer* vertexBuffer = mesh->getVertexBuffer();
		getContext().IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		getContext().IASetIndexBuffer(mesh->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		for (VirtualScreen* screen : virtualScreens)
		{
			if (screen) {
				Camera* cam = screen->getActiveCamera();
				if (cam) {
					ID3D11RenderTargetView* rtv = screen->getRenderTargetView();
					getContext().OMSetRenderTargets(1, &rtv, screen->getDepthStencilView());

					// update constant buffer (matrices)
					VS_CONSTANT_BUFFER data;
					XMStoreFloat4x4(&data.mWorldViewProj, XMMatrixTranspose(cam->getViewProjMatrix()));
					XMStoreFloat4x4(&data.mWorldViewProjInv, XMMatrixInverse(nullptr, cam->getViewProjMatrix()));
					vs.updateConstantBuffer(data);

					getContext().DrawIndexed(mesh->getIndexCount(), 0, 0);
				}
			}
		}
	}
	for (VirtualScreen* screen : virtualScreens)
	{
		if (screen)
		{
			screen->present();
		}
	}
}
