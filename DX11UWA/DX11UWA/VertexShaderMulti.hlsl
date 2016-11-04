// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
	float4 WorldPos : WORLDPOS;
	float4 tangent : TANGENT;
	float4 bitangent : BiTANGENT;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	float4 posWorld;

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	posWorld = pos;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// Pass the color through without modification.
	output.color = input.color;

	//per-pixel light require
	output.normal = mul(input.normal, model);
	output.WorldPos = posWorld;
	output.tangent = mul(float4(input.tangent.xyz * input.tangent.w, 0.0f), model);
	output.bitangent = mul(float4(cross(input.normal.xyz, input.tangent.xyz), 0.0f), model);

	return output;
}