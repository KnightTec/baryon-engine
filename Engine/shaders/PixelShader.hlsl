struct VSOutput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
};

void main(in VSOutput input, out float4 gBuffer0 : SV_Target0, out float4 gBuffer1 : SV_Target1)
{
    // flat shading
	//float3 nor = cross(ddx(input.wPos), ddy(input.wPos));
	//nor = normalize(nor);
    float3 normal = normalize(input.normal);
    gBuffer0 = float4(1, 0, 0, 1);
    gBuffer1 = float4(normal, 1);
}