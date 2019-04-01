cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix viewProj;
};

struct VSInput
{
	float3 pos : POSITION;
	float3 nor : NORMAL;
};
struct VSOutput 
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
};

VSOutput main(in VSInput input)
{
	VSOutput output;
	output.pos = mul(float4(input.pos, 1), viewProj);
	output.nor = input.nor;
	return output;
}