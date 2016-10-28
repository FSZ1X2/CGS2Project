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
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);

	//float dotD = Clamps(pos*DirectionalLight.pos));
	//float dotP = Clamps(pos*(PointLight.pos-pos));
	//unsigned int colorlerpD = ChangeColor3(DirectionalLight.color, BLACK, Clamps(dotD + ambient));
	//unsigned int colorlerpP = ChangeColor3(PointLight.color, BLACK, Clamps(dotP));

	//float ATTENUATION = 1.0 - Clamps(Leng(Sub(PointLight, pos)) / lightradius);
	//unsigned int colorp = MuColor(colorlerpP, ATTENUATION);
	//unsigned int combinecolor = Combine_colors(colorlerpD, colorp);
	//multiplyMe.color = combinecolor;

	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// Pass the color through without modification.
	output.color = input.color;

	return output;
}
