cbuffer PER_OBJECT_DATA : register(b0)
{
    matrix worldViewProjMat;
    matrix worldNormalsMat;
};
cbuffer PER_CAMERA_DATA : register(b1)
{
    float2 screenParams;
    matrix viewProj;
    matrix invViewProj;
    matrix prevViewProj;
    float3 cameraPosition;
    float3 cameraLinearVelocity;
    float3 cameraAngularVelocity;
};

Texture2D<float4> gBufferTexture0 : register(t0);
Texture2D<float4> gBufferTexture1 : register(t1);
Texture2D<float> depthTexture : register(t2);

Texture2D<float4> hdrScene : register(t3);

SamplerState texSampler : register(s0);


float3 getWorldSpacePosition(float2 texCoords)
{
    float3 ndc = float3(texCoords * 2 - 1, depthTexture.Sample(texSampler, texCoords));
    ndc.y *= -1;

    float4 worldPos = mul(float4(ndc, 1), invViewProj);
    worldPos /= worldPos.w;

    return worldPos.xyz;
}