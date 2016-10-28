// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
};

texture2D diffTexture : register(t0);

texture2D normTexture : register(t1);

texture2D specTexture : register(t2);

SamplerState filters[1] : register(s0); 

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 baseColor = diffTexture.Sample(filters[0], input.color.xy); // get base color
	return baseColor; // return a transition based on the detail alpha
}
