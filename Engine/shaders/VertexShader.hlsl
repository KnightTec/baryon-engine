#include "Baryon.hlsl"

struct MeshVertex
{
	float3 position : POSITION;
	float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};
struct VSOutput 
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
};

VSOutput main(in MeshVertex input)
{
	VSOutput output;
    output.position = mul(float4(input.position, 1), worldViewProjMat);
    output.normal = mul(float4(input.normal, 0), worldNormalsMat).xyz;
	return output;
}