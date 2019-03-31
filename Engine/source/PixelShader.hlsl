struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
};

float4 main(in VSOutput input) : SV_Target
{
	return float4(0.098f, 0.439f, 0.439f, 1);
}