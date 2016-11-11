struct GSInput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
	float4 WorldPos : WORLDPOS;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
	float4 WorldPos : WORLDPOS;
};

[maxvertexcount(4)]
void main(
	triangle float4 input[0] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	for (uint i = 0; i < 4; i++)
	{
		GSOutput element;
		element.pos = input[i].pos;
		element.color = input[i].color;
		element.normal = input[i].normal;
		element.WorldPos = input[i].WorldPos;
		output.Append(element);
	}
}