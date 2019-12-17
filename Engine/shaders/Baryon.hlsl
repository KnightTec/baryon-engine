cbuffer PER_OBJECT_DATA : register(b0)
{
    matrix worldViewProjMat;
    matrix worldNormalsMat;
};
cbuffer PER_CAMERA_DATA : register(b1)
{
    matrix invViewProj;
    matrix prevViewProj;
    float3 cameraPosition;
    float4 cameraLinearVelocity;
    float4 cameraAngularVelocity;
};

Texture2D<float4> gBufferTexture0 : register(t0);
Texture2D<float4> gBufferTexture1 : register(t1);
Texture2D<float> depthTexture : register(t2);

Texture2D<float4> hdrScene : register(t3);


SamplerState texSampler : register(s0);