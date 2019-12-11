cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix worldViewProj;
    matrix mWorldNormals;
};

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
	output.pos = mul(float4(input.pos, 1), worldViewProj);
    output.nor = mul(float4(input.nor, 0), mWorldNormals).xyz;
	output.wPos = input.pos;
	return output;
}