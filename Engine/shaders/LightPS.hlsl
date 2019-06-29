Texture2D depthTex : register(t0);
Texture2D normalTex : register(t1);

SamplerState texSampler : register(s0);

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(in VSOutput input) : SV_Target0
{
    float3 nor = normalTex.Sample(texSampler, input.tex).xyz;
    if (length(nor) == 0)
    {
        return float4(0, 0, 0, 1);
    }
    float depth = depthTex.Sample(texSampler, input.tex);

    float3 lightDir = normalize(float3(1, 3, -4));
	return float4(1, 0.85, 0.8, 1) * saturate(dot(lightDir, nor)) + 0.1f * float4(1, 1, 1, 1);
  
    //return float4(float3(1, 1,1) * depth, 1);
}