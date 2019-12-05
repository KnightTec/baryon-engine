#pragma once
#include <DirectXMath.h>

struct VS_CONSTANT_BUFFER
{
	DirectX::XMFLOAT4X4 mWorldViewProj;
	DirectX::XMFLOAT4X4 mWorldNormals;
};

struct PS_CONSTANT_BUFFER
{
	DirectX::XMFLOAT4X4 invViewProj;
	DirectX::XMFLOAT4 cameraPosition;
};

struct POST_PROCESS_CBUFFER
{
	DirectX::XMFLOAT4X4 invViewProj;
	DirectX::XMFLOAT4X4 prevFrameViewProj;
};