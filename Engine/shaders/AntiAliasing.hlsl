#include "Baryon.hlsl"

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

Texture2D<float4> historyFrame : register(t4);
Texture2D<float4> currentFrame : register(t5);

float4 main(in VSOutput input) : SV_Target0
{
    float3 worldPos = getWorldSpacePosition(input.tex);
    
    float4 previousNdc = mul(float4(worldPos, 1), prevViewProj);
    previousNdc /= previousNdc.w;
    float2 ndc = input.tex * 2 - 1;
    ndc.y *= -1;
    float2 velocity = (previousNdc.xy - ndc) / 2;
    velocity.y *= -1;
    
    float3 colorCurrent = currentFrame.Sample(texSampler, input.tex).rgb;
    
    float3 colorHistory = historyFrame.Sample(texSampler, input.tex + velocity).rgb;
   
    float3 finalColor = 0.5 * colorHistory + 0.5 * colorCurrent;
    return float4(finalColor, 1);
}