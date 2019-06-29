struct VSOutput 
{
	float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

void main(uint vertexId : SV_VertexID, out VSOutput output)
{
    output.tex = float2(vertexId >> 1, vertexId % 2) * 2;
    output.pos = float4(output.tex * 2 - 1, 1, 1);
    output.pos.y *= -1;
}