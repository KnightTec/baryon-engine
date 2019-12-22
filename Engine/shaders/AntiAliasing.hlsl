#include "Baryon.hlsl"

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

Texture2D<float4> historyFrame : register(t4);
Texture2D<float4> currentFrame : register(t5);

float3 rayAABBintersection(float3 rayOrigin, float3 rayDirection, float3 minAABB, float3 maxAABB)
{
    
}

float4 main(in VSOutput input) : SV_Target0
{
    float3 worldPos = getWorldSpacePosition(input.tex);
    
    float4 previousNdc = mul(float4(worldPos, 1), prevViewProj);
    previousNdc /= previousNdc.w;
    float2 ndc = input.tex * 2 - 1;
    ndc.y *= -1;
    float2 velocity = (previousNdc.xy - ndc) / 2;
    velocity.y *= -1;
    
    //TODO: improve silhouttes
    
    float3 colorCurrent = currentFrame.Sample(texSampler, input.tex).rgb; 
    
    float2 historySampleTexCoord = input.tex + velocity;
    
    float3 colorHistory = historyFrame.Sample(texSampler, historySampleTexCoord).rgb;
    
    // variance clipping https://developer.download.nvidia.com/gameworks/events/GDC2016/msalvi_temporal_supersampling.pdf
    float3 m1 = float3(0, 0, 0);
    float3 m2 = float3(0, 0, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            if (x == 0 && y == 0)
            {
                continue;
            }
            float2 offset = float2(x, y) * screenParams;
            //TODO: bicubic sampling
            float3 color = currentFrame.Sample(texSampler, input.tex + offset);
            color = saturate(color);
            m1 += color;
            m2 += color * color;
        }
    }
    float3 mean = m1 / 8.0f;
    float3 sigma = sqrt(abs(m2 / 8.0f - mean * mean));
    float3 minColor = mean - sigma;
    float3 maxColor = mean + sigma;
        
    //TODO: perform clipping instead of clamping
    colorHistory = clamp(colorHistory, minColor, maxColor);
    
    
      
    float3 finalColor = 0.1 * colorCurrent + 0.9 * colorHistory;
    return float4(finalColor, 1);
}