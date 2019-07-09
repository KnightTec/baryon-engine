Texture2D depthTex : register(t0);
Texture2D sceneTex : register(t1);

SamplerState texSampler : register(s0);

cbuffer CONSTANT_DATA : register(b0)
{
    matrix invViewProj;
    matrix prevViewProj;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float3 getNDC(float2 texCoords)
{
    float3 ndc;
    ndc.z = depthTex.Sample(texSampler, texCoords);
    ndc.xy = texCoords * 2 - 1;
    ndc.y *= -1;
    return ndc;
}

float3 getWorldPos(float2 texCoords)
{
    float3 ndc = getNDC(texCoords);

    float4 worldPos = mul(float4(ndc, 1), invViewProj);
    worldPos /= worldPos.w;

    return worldPos.xyz;
}

float4 main(in VSOutput input) : SV_Target0
{
    // perform camera motion blur
    float3 worldPos = getWorldPos(input.tex);
    float4 previousNdc = mul(float4(worldPos, 1), prevViewProj);
    previousNdc /= previousNdc.w;

    float3 ndc = getNDC(input.tex);

    float2 velocity = (ndc.xy - previousNdc.xy)/20.0f;

    int numSamples = 6;
    float3 color = sceneTex.Sample(texSampler, input.tex).xyz;
    input.tex += velocity;

    for (int i = 1; i < numSamples; ++i, input.tex += velocity) {
        color += sceneTex.Sample(texSampler, input.tex).xyz;
    }

    float3 finalColor = color / numSamples;
    
    return float4(finalColor, 1);
}