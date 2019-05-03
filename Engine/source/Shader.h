#pragma once
#include "GraphicsDeviceInterface.h"

#include <string>
#include <vector>

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
	 * Update the data in the constant buffer specified by its index
	 */
	bool updateConstantBufferByIndex(void* data, uint32_t dataSizeInBytes, uint32_t index);
protected:
	std::wstring sourcePath;
private:
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> cbuffers;
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
	if (!compile())
	{
		return false;
	}
	apply();
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
