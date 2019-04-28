#pragma once
#include "GraphicsDeviceInterface.h"
#include "DXErr.h"

#include <string>


namespace Baryon
{
struct Empty
{
};


class Shader : protected GraphicsDeviceInterface
{
public:
	Shader(const std::wstring& sourcePath);
	virtual bool compile() = 0;
	virtual void apply() = 0;
	bool reload();
	template<typename CONSTANT_BUFFER>
	bool updateConstantBuffer(const CONSTANT_BUFFER& constantBuffer);
protected:
	std::wstring sourcePath;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> cbuffer;
	bool initCBuffer = false;
};

class VertexShader : public Shader
{
public:
	using Shader::Shader;
	/*
	 * Compile HLSL vertex shader file
	 */
	bool compile() override;
	/*
	 * Apply this vertex shader to the rendering pipeline
	 */
	void apply() override;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> d3dvertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> d3dinputLayout;
};

class PixelShader : public Shader
{
public:
	using Shader::Shader;
	/*
	 * Compile HLSL pixel shader file
	 */
	bool compile() override;
	/*
	 * Apply this pixel shader to the rendering pipeline
	 */
	void apply() override;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> d3dpixelShader;
};

class RenderPass : GraphicsDeviceInterface
{
public:
	RenderPass();
	void apply();
	bool setVertexShader(const VertexShader* vertexShader);
	bool setPixelShader(const PixelShader* pixelShader);

	VertexShader* getVertexShader() const;
	PixelShader* getPixelShader() const;
private:
	VertexShader* vertexShader;
	PixelShader* pixelShader;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// Inline function implementations 
///////////////////////////////////////////////////////////////////////////////////////////////
inline Shader::Shader(const std::wstring& sourcePath) : sourcePath{sourcePath}
{
}

inline bool Shader::reload()
{
	bool success = compile();
	apply();
	return success;
}

template <typename CONSTANT_BUFFER>
bool Shader::updateConstantBuffer(const CONSTANT_BUFFER& constantBuffer)
{
	if (!initCBuffer)
	{
		CONSTANT_BUFFER constData = {};

		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &constData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		HR(getDevice().CreateBuffer(&cbDesc, &initData, cbuffer.GetAddressOf()));
		getContext().VSSetConstantBuffers(0, 1, cbuffer.GetAddressOf());

		initCBuffer = true;
	}
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(getContext().Map(cbuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	auto* data = reinterpret_cast<CONSTANT_BUFFER*>(mappedData.pData);
	*data = constantBuffer;
	getContext().Unmap(cbuffer.Get(), 0);
	return true;
}

inline void VertexShader::apply()
{
	getContext().IASetInputLayout(d3dinputLayout.Get());
	getContext().VSSetShader(d3dvertexShader.Get(), nullptr, 0);
}

inline void PixelShader::apply()
{
	getContext().PSSetShader(d3dpixelShader.Get(), nullptr, 0);
}

inline RenderPass::RenderPass() : vertexShader{nullptr}, pixelShader{nullptr}
{
}

inline void RenderPass::apply()
{
	vertexShader->apply();
	pixelShader->apply();
}
} // namespace Baryon
