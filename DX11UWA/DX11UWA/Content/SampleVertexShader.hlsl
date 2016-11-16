// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model[4];
	matrix view;
	matrix projection;
	float4 Camera;
	//matrix lightViewMatrix;
	//matrix lightProjectionMatrix;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
	float4 WorldPos : WORLDPOS;
	float4 CamWorldPos : CAMWORLDPOS;
	//float4 lightViewPosition : TEXCOORD1;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input, uint index: SV_InstanceID)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	float4 posWorld;

	// Transform the vertex position into projected space.
	pos = mul(pos, model[index]);
	posWorld = pos;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	//Calculate the position of the vertice as viewed by the light source.
	//output.lightViewPosition = mul(pos, model[index]);
	//output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	//output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// Pass the color through without modification.
	output.color = input.color;

	//per-pixel light require
	output.normal = mul(input.normal, model[index]);
	output.WorldPos = posWorld;
	output.CamWorldPos = Camera;

	return output;
}
