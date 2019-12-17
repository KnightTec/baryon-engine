#pragma once
#include "GraphicsDeviceInterface.h"
#include "DXErr.h"
#include "Singleton.h"

#include "wrl/client.h"
#include <cassert>
#include <cstdint>

namespace Baryon
{
enum SHADER_STAGE : uint8_t
{
	COMPUTE_SHADER = 0x01,
	VERTEX_SHADER = 0x02,
	HULL_SHADER = 0x04,
	DOMAIN_SHADER = 0x08,
	GEOMETRY_SHADER = 0x10,
	PIXEL_SHADER = 0x20
};


template <typename T>
class ConstantBuffer : public Singleton<ConstantBuffer<T>>, GraphicsDeviceInterface
{
	friend class Singleton<ConstantBuffer<T>>;
public:
	void initialize(int registerNumber, uint8_t shaderStages);
	void uploadBuffer();
	T* operator->();
private:
	ConstantBuffer() = default;

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	T data;
};

#define cBuffer(T) ConstantBuffer<T>::get()


template <typename T>
void ConstantBuffer<T>::initialize(int registerNumber, uint8_t shaderStages)
{
	assert(registerNumber <= D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT);

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof T;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	T dummyData;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &dummyData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRF(getDevice()->CreateBuffer(&cbDesc, &initData, buffer.GetAddressOf()));

	if (shaderStages & COMPUTE_SHADER) {
		getContext()->CSSetConstantBuffers(registerNumber, 1, buffer.GetAddressOf());
	}
	if (shaderStages & VERTEX_SHADER) {
		getContext()->VSSetConstantBuffers(registerNumber, 1, buffer.GetAddressOf());
	}
	if (shaderStages & HULL_SHADER) {
		getContext()->HSSetConstantBuffers(registerNumber, 1, buffer.GetAddressOf());
	}
	if (shaderStages & DOMAIN_SHADER) {
		getContext()->DSSetConstantBuffers(registerNumber, 1, buffer.GetAddressOf());
	}
	if (shaderStages & GEOMETRY_SHADER) {
		getContext()->GSSetConstantBuffers(registerNumber, 1, buffer.GetAddressOf());
	}
	if (shaderStages & PIXEL_SHADER) {
		getContext()->PSSetConstantBuffers(registerNumber, 1, buffer.GetAddressOf());
	}
}
template <typename T>
void ConstantBuffer<T>::uploadBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	getContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof T, &data, sizeof T);
	getContext()->Unmap(buffer.Get(), 0);
}
template <typename T>
T* ConstantBuffer<T>::operator->()
{
	return &data;
}

}
