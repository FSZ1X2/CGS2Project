// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
	float4 WorldPos : WORLDPOS;
};

texture2D diffTexture : register(t0);

texture2D normTexture : register(t1);

texture2D specTexture : register(t2);

SamplerState filters[3] : register(s0); 

static float3 DirectionalLight = { 0.0f,5.0f,0.0f };// : register(l0);
static float3 PointLightPosition = { 0.0f,0.0f,-1.0f };// : register(l1);
static float3 DLcolor = { 1.0f,1.0f,1.0f };// : register(c0);
static float3 PLcolor = { 0.0f,0.0f,1.0f };// : register(c1);
static float lightradius = 2.0f;// : register(r0);

//float3 LightPosition = { 0.0f,0.0f,0.0f };
//float3 LightDiffuseColor = { 0.5f,0.5f,0.5f }; // intensity multiplier
//float3 LightSpecularColor = { 1.0f,0.5f,0.0f }; // intensity multiplier
//float LightDistanceSquared = 1.0f;
//float3 AmbientLightColor = { 1.0f,0.5f,0.5f };
//float3 EmissiveColor = { 0.0f,0.5f,0.0f };
//float3 SpecularColor = { 0.0f,0.5f,0.5f };
//float SpecularPower = 2.0f;

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 lightDirP = normalize(PointLightPosition - input.WorldPos);

	float dotD = clamp(dot(input.normal, DirectionalLight), 0, 1);
	float dotP = clamp(dot(input.normal, lightDirP), 0, 1);
	
	float4 dcolor = float4(DLcolor, 1.0f)*clamp((dotD + 0.2f),0,1);
	float4 pcolor = float4(PLcolor, 1.0f)*dotP;
					
	float ATTENUATION = 1.0 - clamp((lightDirP / lightradius), 0, 1);
	
	float4 pcolor2 = pcolor * ATTENUATION;
	float4 combinecolor = clamp(dcolor + pcolor2, 0, 1);
	
	float4 baseColor = diffTexture.Sample(filters[0], input.color.xy) * combinecolor; // get base color

	//float4 addnorm = normTexture.Sample(filters[1], input.color.xy);
	//float4 addspec = specTexture.Sample(filters[2], input.color.xy);
	//float4 color = lerp(addnorm, baseColor, baseColor.a);
	//float4 colorfinal = lerp(addspec, color, color.a);
	//return colorfinal;
	//lighting:
	//float3 lightDir = normalize(input.WorldPos - LightPosition); // per pixel diffuse lighting
	//float diffuseLighting = saturate(dot(input.normal, -lightDir));
	//diffuseLighting *= (LightDistanceSquared / dot(LightPosition - input.WorldPos, LightPosition - input.WorldPos));
	//float3 h = normalize(normalize(CameraPos - input.WorldPos) - lightDir);
	//float specLighting = pow(saturate(dot(h, input.normal)), SpecularPower);
	//float4 texel = diffTexture.Sample(filters[0], input.color.xy);//texture2D(filters[0], input.color.xy);
	//float4 baseColor = diffTexture.Sample(filters[0], input.color.xy);
	//return float4(saturate(
	//	AmbientLightColor +
	//	(texel.xyz * baseColor * LightDiffuseColor * diffuseLighting * 0.6) + // Use light diffuse vector as intensity multiplier
	//	(SpecularColor * LightSpecularColor * specLighting * 0.5) // Use light specular vector as intensity multiplier
	//), texel.w);

	return baseColor;// baseColor; // return a transition based on the detail alpha
}
