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
	explicit Shader(const std::wstring& sourcePath);
	/*
	 * Compile HLSL vertex shader file located at sourcePath
	 */
	virtual bool compile() = 0;
	/*
	 * Apply this shader to the rendering pipeline
	 * so it is used in the next draw call
	 */
	virtual void apply() = 0;
	/*
	 * Compile and apply this shader
	 */
	bool reload();
	/*
	 * Update the data in the constant buffer
	 */
	template<typename CONSTANT_BUFFER>
	bool updateConstantBuffer(const CONSTANT_BUFFER& data);
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
bool Shader::updateConstantBuffer(const CONSTANT_BUFFER& data)
{
	
	if (!initCBuffer)
	{
		// create the constant buffer
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
	// update the constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(getContext().Map(cbuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));;
	auto* cbData = reinterpret_cast<CONSTANT_BUFFER*>(mappedData.pData);
	*cbData = data;
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
} // namespace Baryon
