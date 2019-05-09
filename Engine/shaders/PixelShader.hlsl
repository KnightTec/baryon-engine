struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
	float3 wPos : POSITION;
};

float4 main(in VSOutput input) : SV_Target
{
	float3 nor = cross(ddx(input.wPos), ddy(input.wPos));
	nor = normalize(nor);

	float3 lightDir = float3(1, 3, -4);
	lightDir = normalize(lightDir);
	return float4(1, 0.85, 0.8, 1) * saturate(dot(lightDir, nor)) + 0.1f * float4(0.8, 0.8, 1, 1);
}