// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
};

textureCUBE SkyBoxTexture : register(t0);
SamplerState sky;

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	return SkyBoxTexture.Sample(sky, input.color);
}
