RWTexture2D<float2> output : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    output[id.xy] = float2(0.1, 0.7);
}