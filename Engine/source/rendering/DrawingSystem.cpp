#include "DrawingSystem.h"
#include "Shader.h"
#include "GpuData.h"
#include "DXErr.h"
#include "VirtualScreen.h"
#include "ConstantBuffer.h"

#include "DirectXMath.h"

using namespace Baryon;
using namespace DirectX;
using namespace Microsoft::WRL;


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

	TAA.compile();
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
	{
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_KEEP,
		D3D11_COMPARISON_ALWAYS
	};

	dsDesc.FrontFace = defaultStencilOp;
	dsDesc.BackFace = defaultStencilOp;

	HRF(getDevice()->CreateDepthStencilState(&dsDesc, depthStencilState.GetAddressOf()));

	getContext()->OMSetDepthStencilState(depthStencilState.Get(), 0);

	/*
	 * TODO: Blend State
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

	postVS.apply();
	getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	for (VirtualScreen& screen : virtualScreens)
	{
		CameraComponent* cam = screen.getActiveCamera();
		if (cam == nullptr)
		{
			continue;
		}

		auto& cameraData = cBuffer(PER_CAMERA_DATA);
		cameraData->screenResolution = screen.getResolution();
		cameraData->viewProjection = cam->getViewProjectionXM();
		cameraData->invViewProj = XMMatrixInverse(nullptr, cam->getViewProjectionXM());
		cameraData->cameraPosition = cam->position;
		cameraData->cameraLinearVelocity = cam->linearVelocity;
		cameraData.uploadBuffer();
		cameraData->prevFrameViewProjMat = cam->getViewProjectionXM();

		// render light pass
		lightPS.apply();
		screen.setupIntermediateViewport();
		screen.setupLightPass();
		getContext()->Draw(3, 0);

		// anti-aliasing
		TAA.apply();
		screen.setupAAPass();
		getContext()->Draw(3, 0);

		// render post processing
		postPS.apply();
		screen.setupFinalViewport();
		screen.setupPostProcessPass();
		getContext()->Draw(3, 0);
		screen.present();
	}

	ID3D11ShaderResourceView* nulls[] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	getContext()->PSSetShaderResources(0, 8, nulls);
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
		CameraComponent* cam = screen.getActiveCamera();
		if (cam == nullptr)
		{
			continue;
		}
		screen.setupIntermediateViewport();
		screen.setupGeometryPass();

		// update constant buffer (matrices)
		auto& buffer = cBuffer(PER_OBJECT_DATA);
		buffer->worldViewProjMat = worldMatrix * cam->getViewProjectionXM() * screen.getTaaJitterMatrix();
		buffer->worldNormalsMat = XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));
		buffer.uploadBuffer();

		getContext()->DrawIndexed(mesh.mesh->getIndexCount(), 0, 0);
	}
}
