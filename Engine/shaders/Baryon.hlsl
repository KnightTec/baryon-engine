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