#include "Shader.h"
#include "DXErr.h"

#include "d3dcompiler.h"

#include <vector>

using namespace Baryon;
using namespace Microsoft::WRL;

bool Shader::updateConstantBufferByIndex(void* data, uint32_t dataSizeInBytes, uint32_t index)
{
	if (!initCBuffer)
	{
		cbuffers.emplace_back(Microsoft::WRL::ComPtr<ID3D11Buffer>());
		// create the constant buffer
		char* dummyData = new char[dataSizeInBytes];

		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = dataSizeInBytes;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &initData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		HR(getDevice()->CreateBuffer(&cbDesc, &initData, cbuffers[index].GetAddressOf()));
		getContext()->VSSetConstantBuffers(0, 1, cbuffers[index].GetAddressOf());

		delete[] dummyData;

		initCBuffer = true;
	}
	// update the constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(getContext()->Map(cbuffers[index].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, dataSizeInBytes, data, dataSizeInBytes);
	getContext()->Unmap(cbuffers[index].Get(), 0);
	return true;
}

bool VertexShader::compile()
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif
	ComPtr<ID3DBlob> shaderBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HR(D3DCompileFromFile(sourcePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0,
		shaderBlob.GetAddressOf(), errorBlob.GetAddressOf()));
	HR(getDevice()->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr,
		d3dvertexShader.GetAddressOf()));

	// Reflect shader info
	ComPtr<ID3D11ShaderReflection> vertexShaderReflection;
	HR(D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), __uuidof(ID3D11ShaderReflection),
		reinterpret_cast<void**>(vertexShaderReflection.GetAddressOf())));

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	vertexShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	const uint32_t inputParamCount = shaderDesc.InputParameters;
	for (uint32_t i = 0; i < inputParamCount; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		vertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// Determine DXGI format
		if (paramDesc.Mask == 1)
		{
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
			default:
				break;
			}
		}
		else if (paramDesc.Mask <= 3)
		{
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			default:
				break;
			}
		}
		else if (paramDesc.Mask <= 7)
		{
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			default:
				break;
			}
		}
		else if (paramDesc.Mask <= 15)
		{
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			default:
				break;
			}
		}
		inputLayoutDesc.push_back(elementDesc);
	}
	HR(getDevice()->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), shaderBlob->GetBufferPointer(),
	                              shaderBlob->GetBufferSize(), d3dinputLayout.GetAddressOf()));
	return true;
}

bool PixelShader::compile()
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif
	ComPtr<ID3DBlob> shaderBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HR(D3DCompileFromFile(sourcePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags, 0,
		shaderBlob.GetAddressOf(), errorBlob.GetAddressOf()));
	HR(getDevice()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr,
		d3dpixelShader.GetAddressOf()));
	return true;
}
