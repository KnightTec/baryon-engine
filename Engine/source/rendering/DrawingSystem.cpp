#include "DrawingSystem.h"
#include "Shader.h"
#include "GpuData.h"
#include "DXErr.h"
#include "../Camera.h"
#include "VirtualScreen.h"

#include "DirectXMath.h"

using namespace Baryon;
using namespace DirectX;
using namespace Microsoft::WRL;
	

static POST_PROCESS_CBUFFER postProcessData = {};

static ComPtr<ID3D11RasterizerState1> rasterState;
static ComPtr<ID3D11SamplerState> samplerState;
static ComPtr<ID3D11DepthStencilState> depthStencilState;


DrawingSystem::DrawingSystem(EntityManager* entityManager, std::vector<VirtualScreen>& virtualScreens)
	: super(entityManager), virtualScreens(virtualScreens)
{
}
void DrawingSystem::initialize()
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
	HRF(getDevice()->CreateRasterizerState1(&rasterizerState, rasterState.GetAddressOf()));
	getContext()->RSSetState(rasterState.Get());

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	HRF(getDevice()->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf()));

	getContext()->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	const D3D11_DEPTH_STENCILOP_DESC defaultStencilOp =
	{ D3D11_STENCIL_OP_KEEP,
	  D3D11_STENCIL_OP_KEEP,
	  D3D11_STENCIL_OP_KEEP,
	  D3D11_COMPARISON_ALWAYS };

	dsDesc.FrontFace = defaultStencilOp;
	dsDesc.BackFace = defaultStencilOp;

	HRF(getDevice()->CreateDepthStencilState(&dsDesc, depthStencilState.GetAddressOf()));

	getContext()->OMSetDepthStencilState(depthStencilState.Get(), 0);

	/*
	 * TODO: Blend State
	  Depth Stencil State
	  Sampler State
	 */
}
void DrawingSystem::terminate()
{

}
void DrawingSystem::tick()
{
	vs.apply();
	ps.apply();
	for (VirtualScreen& screen : virtualScreens)
	{
		screen.clear();
	}

	// render geometry pass
	getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	super::tick();

	// render light pass
	postVS.apply();
	lightPS.apply();

	//getContext()->IASetInputLayout(nullptr);
	getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (VirtualScreen& screen : virtualScreens)
	{
		Camera* cam = screen.getActiveCamera();
		if (cam != nullptr) {

			PS_CONSTANT_BUFFER data = {};
			XMStoreFloat4x4(&data.invViewProj, XMMatrixTranspose(XMMatrixInverse(nullptr, cam->getViewProjMatrix())));
			XMStoreFloat4(&data.cameraPosition, cam->getPostion());
			lightPS.updateConstantBufferByIndex(&data, sizeof(data), 0);

			screen.setupIntermediateViewport();
			screen.setupLightPass();

			ID3D11ShaderResourceView* srvs[] = { screen.depthBufferSRV.Get(), screen.worldNormals.getShaderResourceView() };
			getContext()->PSSetShaderResources(0, 2, srvs);

			getContext()->Draw(3, 0);
		}
	}

	// render post processing
	postPS.apply();
	for (VirtualScreen& screen : virtualScreens)
	{
		Camera* cam = screen.getActiveCamera();
		if (cam != nullptr) {
			XMStoreFloat4x4(&postProcessData.invViewProj, XMMatrixTranspose(XMMatrixInverse(nullptr, cam->getViewProjMatrix())));
			postPS.updateConstantBufferByIndex(&postProcessData, sizeof postProcessData, 0);

			screen.setupFinalViewport();
			screen.setupPostProcessPass();
			ID3D11ShaderResourceView* srvs[] = { screen.depthBufferSRV.Get(), screen.litScene.getShaderResourceView() };
			getContext()->PSSetShaderResources(0, 2, srvs);

			getContext()->Draw(3, 0);
			screen.present();

			XMStoreFloat4x4(&postProcessData.prevFrameViewProj, XMMatrixTranspose(cam->getViewProjMatrix()));
		}
	}

	ID3D11ShaderResourceView* nulls[] = { nullptr, nullptr };
	getContext()->PSSetShaderResources(0, 2, nulls);
}
void DrawingSystem::update(WorldMatrixComponent& wmc, MeshComponent& mesh)
{
	if (mesh.mesh == nullptr)
	{
		return;
	}
	XMMATRIX worldMatrix = XMLoadFloat4x3(&wmc.worldMatrix);

	uint32_t strides = sizeof(Vertex);
	uint32_t offsets = 0;
	ID3D11Buffer* vertexBuffer = mesh.mesh->getVertexBuffer();
	getContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
	getContext()->IASetIndexBuffer(mesh.mesh->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	for (VirtualScreen& screen : virtualScreens)
	{
		Camera* cam = screen.getActiveCamera();
		if (cam)
		{
			screen.setupIntermediateViewport();
			screen.setupGeometryPass();

			// update constant buffer (matrices)
			VS_CONSTANT_BUFFER data = {};
			XMStoreFloat4x4(&data.mWorldViewProj, XMMatrixTranspose(worldMatrix * cam->getViewProjMatrix()));
			XMStoreFloat4x4(&data.mWorldNormals, XMMatrixInverse(nullptr, worldMatrix));
			vs.updateConstantBufferByIndex(&data, sizeof(data), 0);

			getContext()->DrawIndexed(mesh.mesh->getIndexCount(), 0, 0);
		}
	}
}
