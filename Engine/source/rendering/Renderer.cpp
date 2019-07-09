#include "Renderer.h"
#include "VirtualScreen.h"
#include "../Mesh.h"
#include "RenderPass.h"
#include "DXErr.h"
#include "../Window.h"

using namespace Baryon;
using namespace DirectX;
using namespace Microsoft::WRL;


struct VS_CONSTANT_BUFFER
{
	XMFLOAT4X4 mWorldViewProj;
	XMFLOAT4X4 mWorldNormals;
};

struct PS_CONSTANT_BUFFER
{
	XMFLOAT4X4 invViewProj;
	XMFLOAT4 cameraPosition;
};

struct POST_PROCESS_CBUFFER
{
	XMFLOAT4X4 invViewProj;
	XMFLOAT4X4 prevFrameViewProj;
};
static POST_PROCESS_CBUFFER postProcessData = {};

ComPtr<ID3D11RasterizerState1> rasterState;
ComPtr<ID3D11SamplerState> samplerState;

static VertexShader vs{L"../../Engine/shaders/VertexShader.hlsl", 1};
static PixelShader ps{L"../../Engine/shaders/PixelShader.hlsl"};

static VertexShader postVS{L"../../Engine/shaders/FullscreenVS.hlsl"};
static PixelShader lightPS{L"../../Engine/shaders/LightPS.hlsl", 1};
static PixelShader postPS{ L"../../Engine/shaders/PostProcessPS.hlsl", 1};

bool Renderer::initialize()
{
	vs.compile();
	ps.compile();

	postVS.compile();
	lightPS.compile();
	postPS.compile();

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

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	HR(getDevice()->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf()));

	getContext()->PSSetSamplers(0, 1, samplerState.GetAddressOf());


	//TODO: invert depth buffer
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
	vs.apply();
	ps.apply();
	for (VirtualScreen& screen : virtualScreens)
	{
		screen.clear();
	}

	// render geometry pass
	getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (const Mesh* mesh : meshes)
	{
		XMMATRIX worldMatrix = mesh->transform.getMatrix();

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
				screen.setupGeometryPass();

				// update constant buffer (matrices)
				VS_CONSTANT_BUFFER data = {};
				XMStoreFloat4x4(&data.mWorldViewProj, XMMatrixTranspose(worldMatrix * cam->getViewProjMatrix()));
				XMStoreFloat4x4(&data.mWorldNormals, XMMatrixInverse(nullptr, worldMatrix));
				vs.updateConstantBufferByIndex(&data, sizeof(data), 0);

				getContext()->DrawIndexed(mesh->getIndexCount(), 0, 0);
			}
		}
	}

	// render light pass
	postVS.apply();
	lightPS.apply();

	getContext()->IASetInputLayout(nullptr);
	getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (VirtualScreen& screen : virtualScreens)
	{
		Camera* cam = screen.getActiveCamera();

		PS_CONSTANT_BUFFER data = {};
		XMStoreFloat4x4(&data.invViewProj, XMMatrixTranspose(XMMatrixInverse(nullptr, cam->getViewProjMatrix())));
		XMStoreFloat4(&data.cameraPosition, cam->getPostion());
		lightPS.updateConstantBufferByIndex(&data, sizeof(data), 0);

		screen.setupLightPass();

		ID3D11ShaderResourceView* srvs[] = {screen.depthBufferSRV.Get(), screen.worldNormals.getShaderResourceView()};
		getContext()->PSSetShaderResources(0, 2, srvs);
		
		getContext()->Draw(3, 0);
		//screen.present();
	}

	// render post processing
	postPS.apply();
	for (VirtualScreen& screen : virtualScreens)
	{
		
		XMStoreFloat4x4(&postProcessData.invViewProj, XMMatrixTranspose(XMMatrixInverse(nullptr, screen.getActiveCamera()->getViewProjMatrix())));
		postPS.updateConstantBufferByIndex(&postProcessData, sizeof postProcessData, 0);

		screen.setupPostProcessPass();
		ID3D11ShaderResourceView* srvs[] = { screen.depthBufferSRV.Get(), screen.litScene.getShaderResourceView() };
		getContext()->PSSetShaderResources(0, 2, srvs);

		getContext()->Draw(3, 0);
		screen.present();

		XMStoreFloat4x4(&postProcessData.prevFrameViewProj, XMMatrixTranspose(screen.getActiveCamera()->getViewProjMatrix()));
	}

	ID3D11ShaderResourceView* nulls[] = {nullptr, nullptr};
	getContext()->PSSetShaderResources(0, 2, nulls);
}
