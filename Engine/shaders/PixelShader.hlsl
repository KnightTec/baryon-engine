struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float3 wPos : POSITION;
};

float4 main(in VSOutput input) : SV_Target0
{
	float3 nor = cross(ddx(input.wPos), ddy(input.wPos));
	nor = normalize(nor);
    return float4(nor, 1);
}