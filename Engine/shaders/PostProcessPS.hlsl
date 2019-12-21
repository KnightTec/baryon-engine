#include "Baryon.hlsl"

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float InterleavedGradientNoise(float2 uv)
{
    float3 magic = { 0.06711056, 0.00583715, 52.9829189 };
    return frac(magic.z * frac(dot(uv, magic.xy)));
}

float4 main(in VSOutput input) : SV_Target0
{
    float3 color = hdrScene.Sample(texSampler, input.tex).xyz;

    // perform camera motion blur
    float3 worldPos = getWorldSpacePosition(input.tex);    
    
    float4 previousNdc = mul(float4(worldPos, 1), prevViewProj);
    previousNdc /= previousNdc.w;
    float2 ndc = input.tex * 2 - 1;
    ndc.y *= -1;
    float2 velocity = (previousNdc.xy - ndc) / 2;
    velocity.y *= -1;
    
    int numSamples = 64;
    float intensity = 0.5;
    velocity /= numSamples;
    velocity *= intensity;
    float2 texPos = input.tex + velocity;
    float2 texNeg = input.tex - velocity;
    float sum = 1;
    for (int i = 1; i < numSamples; ++i, texPos += velocity, texNeg -= velocity)
    {
        if (texPos.x > 0 && texPos.y > 0 && texPos.x < 1 && texPos.y < 1)
        {
            color += hdrScene.Sample(texSampler, texPos).xyz;
            sum += 1;
        }
        if (texNeg.x > 0 && texNeg.y > 0 && texNeg.x < 1 && texNeg.y < 1)
        {
            color += hdrScene.Sample(texSampler, texNeg).xyz;
            sum += 1;
        }
    }
    color = color / sum;

    // add vignette
    float distanceFromCenter = length(input.tex - float2(0.5, 0.5));
    float x = saturate(distanceFromCenter - 0.05);
    float vignette = pow(x, 8) * 24;
    color = color * (1 - vignette);

    // apply dithering (http://enbseries.enbdev.com/forum/viewtopic.php?f=7&t=5220)
    float dither_amp = 0.1;
    float noise = lerp(-0.5, 0.5, InterleavedGradientNoise(input.pos.xy)) * dither_amp;
    //color.xyz = pow(color.xyz, 1.0 / 2.2);
    color.xyz = color.xyz + noise * min(color.xyz + 0.5 * pow(1.0 / 255.0, 2.2), 0.75 * (pow(256.0 / 255.0, 2.2) - 1.0));
    //color.xyz = pow(color.xyz, 2.2);
    
    return float4(color, 1);
}