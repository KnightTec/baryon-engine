#include "Baryon.hlsl"

struct VSInput
{
	float3 pos : POSITION;
	float3 nor : NORMAL;
    float2 tex : TEXCOORD;
};
struct VSOutput 
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float3 wPos : POSITION;
};

VSOutput main(in VSInput input)
{
	VSOutput output;
    output.pos = mul(float4(input.pos, 1), worldViewProjMat);
    output.nor = mul(float4(input.nor, 0), worldNormalsMat).xyz;
	output.wPos = input.pos;
	return output;
}