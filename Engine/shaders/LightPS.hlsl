Texture2D depthTex : register(t0);
Texture2D normalTex : register(t1);

SamplerState texSampler : register(s0);

cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    matrix invViewProj;
    float3 cameraPosition;
};


struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float3 getWorldPos(float2 texCoords)
{
    float3 ndc;
    ndc.z = depthTex.Sample(texSampler, texCoords);
    ndc.xy = texCoords * 2 - 1;
    ndc.y *= -1;

    float4 worldPos = mul(float4(ndc, 1), invViewProj);
    worldPos /= worldPos.w;

    return worldPos.xyz;
}

float4 main(in VSOutput input) : SV_Target0
{
    float3 nor = normalTex.Sample(texSampler, input.tex).xyz;
    
    float3 lightDir = normalize(float3(1, 3, -4));
	float3 light = float3(1, 0.85, 0.8) * saturate(dot(lightDir, nor)) + 0.1f * float3(1, 1, 1);
  
    float3 litColor = float3(0.4, 0.4, 0.4) * light;
    if (length(nor) == 0)
    {
        litColor = float3(0.01, 0.01, 0.01);
    }


    float3 worldPos = getWorldPos(input.tex);
    float3 viewDir = worldPos - cameraPosition;
    float distance = length(viewDir);
    viewDir = normalize(viewDir);
    float falloff = 0.3;

    float fogFactorDepth = 1 - exp(-distance * falloff);
    
    float fogFactorHeight = exp(-cameraPosition.y * falloff) * (1.0 - exp(-distance * viewDir.y * falloff)) / (viewDir.y * falloff);
    fogFactorHeight = saturate(fogFactorHeight);

    float fogFactor = fogFactorDepth * fogFactorHeight;

    float3 outColor = fogFactor * float3(0.8, 0.8, 0.8) + (1 - fogFactor) * litColor;
    return float4(outColor, 1);
}