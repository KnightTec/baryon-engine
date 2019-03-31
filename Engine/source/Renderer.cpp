#include "Renderer.h"
#include "VirtualScreen.h"
#include "Mesh.h"

#include <fstream>

using namespace Baryon;
using namespace DirectX;
using namespace Microsoft::WRL;


struct VS_CONSTANT_BUFFER
{
	XMFLOAT4X4 mWorldViewProj;
};

ComPtr<ID3D11Buffer> cbuffer;
ComPtr<ID3D11VertexShader> vertexShader;
ComPtr<ID3D11InputLayout> inputLayout;
ComPtr<ID3D11PixelShader> pixelShader;

ComPtr<ID3D11RasterizerState1> g_pRasterState;

bool Renderer::initialize()
{
	// create and set vertex shader
	std::ifstream stream{ "../x64/Debug/VertexShader.cso", std::ios::in | std::ios::binary | std::ios::ate };
	if (stream) {

		// get length of file:
		stream.seekg(0, std::ifstream::end);
		size_t length = stream.tellg();
		stream.seekg(0, std::ifstream::beg);

		auto* buffer = new char[length];
		stream.read(buffer, length);

		assert(SUCCEEDED(getDevice().CreateVertexShader(buffer, length, nullptr, vertexShader.GetAddressOf())));

		// create and set input layout
		D3D11_INPUT_ELEMENT_DESC desc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		getDevice().CreateInputLayout(desc, ARRAYSIZE(desc), buffer, length, inputLayout.GetAddressOf());

		getContext().IASetInputLayout(inputLayout.Get());
		getContext().VSSetShader(vertexShader.Get(), nullptr, 0);

		delete[] buffer;
	}
	stream.close();
	
	// create pixel shader
	std::ifstream stream2{ "../x64/Debug/PixelShader.cso", std::ios::in | std::ios::binary | std::ios::ate };
	if (stream2) {
		// get length of file:
		stream2.seekg(0, std::ifstream::end);
		int length = stream2.tellg();
		stream2.seekg(0, std::ifstream::beg);

		auto* buffer = new char[length];
		stream2.read(buffer, length);

		assert(SUCCEEDED(getDevice().CreatePixelShader(buffer, length, nullptr, pixelShader.GetAddressOf())));
		getContext().PSSetShader(pixelShader.Get(), nullptr, 0);

		delete[] buffer;
	}


	// create constant buffer
	VS_CONSTANT_BUFFER VsConstData;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VsConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	getDevice().CreateBuffer(&cbDesc, &InitData, cbuffer.GetAddressOf());
	getContext().VSSetConstantBuffers(0, 1, cbuffer.GetAddressOf());

	getContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create rasterizer state


	D3D11_RASTERIZER_DESC1 rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_NONE;
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
	  Rasterizer State
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
			const float clearColor[] = { 0.739f, 0.339f, 0.139f, 1.000f };
			getContext().ClearRenderTargetView(screen->getRenderTargetView(), clearColor);
			getContext().ClearDepthStencilView(screen->getDepthStencilView(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}
	for (const Mesh* mesh : meshes) {
		UINT strides = sizeof(Vertex);
		UINT offsets = 0;
		ID3D11Buffer* vertexBuffer = mesh->getVertexBuffer();
		//getContext().IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		//getContext().IASetIndexBuffer(mesh->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		getContext().IASetInputLayout(nullptr);
		ID3D11Buffer* vbs[] = { nullptr, };
		unsigned int stride[] = { 0, }, offset[] = { 0, };
		getContext().IASetVertexBuffers(0, 1, vbs, stride, offset);
		getContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (VirtualScreen* screen : virtualScreens)
		{
			if (screen) {
				Camera* cam = screen->getActiveCamera();
				if (cam) {
					ID3D11RenderTargetView* rtv = screen->getRenderTargetView();
					getContext().OMSetRenderTargets(1, &rtv, screen->getDepthStencilView());

					//TODO: update constant buffer (matrices)
					D3D11_MAPPED_SUBRESOURCE mappedData;
					getContext().Map(cbuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
					auto* v = reinterpret_cast<VS_CONSTANT_BUFFER*>(mappedData.pData);
					XMStoreFloat4x4(&v->mWorldViewProj, cam->getViewProjMatrix());
					getContext().Unmap(cbuffer.Get(), 0);

					//getContext().DrawIndexed(mesh->getIndexCount(), 0, 0);
					getContext().Draw(3, 0);
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
