cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix viewProj;
};

struct VSInput
{
	float3 pos : POSITION;
	float3 nor : NORMAL;
};
struct VSOutput 
{
	float4 pos : SV_POSITION;
	float3 nor : NORMAL;
};

/*VSOutput main(in VSInput input)
{
	VSOutput output;
	//output.pos = mul(float4(input.pos, 1), viewProj);
	output.pos = float4(0, 0, 0.1f, 1);
	output.nor = input.nor;
	return output;
}*/
struct VSIn
{
	uint vertexId : SV_VertexID;
};

VSOutput main(VSIn input)
{
	VSOutput output;

	if (input.vertexId == 0)
		output.pos = float4(0.0, 0.5, 0.5, 1.0);
	else if (input.vertexId == 2)
		output.pos = float4(0.5, -0.5, 0.5, 1.0);
	else if (input.vertexId == 1)
		output.pos = float4(-0.5, -0.5, 0.5, 1.0);

	output.nor = float3(1, 0, 0);

	return output;
}