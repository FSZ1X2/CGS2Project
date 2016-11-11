// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : UV;
	float3 normal : NORMAL;
	float4 WorldPos : WORLDPOS;
	float4 tangent : TANGENT;
	float4 bitangent : BiTANGENT;
	//float4 lightViewPosition : TEXCOORD1;
	//float3 lightPos : TEXCOORD2;
};

cbuffer DirectionalLightConstantBuffer : register(b0)
{
	float4 DirectionalLight;
	float4 DLcolor;
};

cbuffer PointLightConstantBuffer : register(b1)
{
	float4 PointLightPosition;
	float4 PLcolor;
	float4 lightradius;
};

cbuffer SpotLightConstantBuffer : register(b2)
{
	float4 SpotLightPosition;
	float4 SLcolor;
	float4 conedir;
	float4 coneratio;
};

texture2D diffTexture : register(t0);

texture2D normTexture : register(t1);

texture2D specTexture : register(t2);

SamplerState filters[3] : register(s0);

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	//float bias;
 //   float4 color;
 //   float2 projectTexCoord;
 //   float depthValue;
 //   float lightDepthValue;
 //   float lightIntensity;
 //   float4 textureColor;

	float3 nc = normTexture.Sample(filters[1], input.color.xy);
	float3 newNormal = (nc*2.0f) - 1.0f;

	float3 normalo = normalize(input.normal);
	float3 tangento = normalize(input.tangent.xyz);
	float3 bitangento = normalize(input.bitangent.xyz);

	float3x3 TBN;
	TBN[0] = tangento.xyz;
	TBN[1] = bitangento.xyz;
	TBN[2] = normalo.xyz;

	newNormal = normalize(mul(newNormal, TBN));

	float3 lightDirP = normalize(PointLightPosition.xyz - input.WorldPos);
	float3 LightDirS = normalize(SpotLightPosition.xyz - input.WorldPos);

	float dotD = clamp(dot(newNormal, normalize(DirectionalLight.xyz)), 0, 1);
	float dotP = clamp(dot(newNormal, lightDirP), 0, 1);
	float dotS = clamp(dot(-LightDirS, normalize(conedir.xyz)), 0, 1);

	float spotfactor = (dotS > coneratio.x) ? 1 : 0;

	spotfactor *= clamp(dot(LightDirS, newNormal), 0, 1);

	float3 dcolor = DLcolor.xyz *clamp((dotD + 0.2f),0,1);
	float3 pcolor = PLcolor.xyz *dotP;
	float3 scolor = spotfactor*SLcolor.xyz;

	float ATTENUATION = 1.0 - clamp((lightDirP / lightradius.x), 0, 1);

	float3 pcolor2 = pcolor * ATTENUATION;
	float3 combinecolor = clamp(dcolor + pcolor2 + scolor, 0, 1);

	//bias = 0.001f;

	//projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	//projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
	//if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	//{
	//	//depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;
	//	lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
	//	lightDepthValue = lightDepthValue - bias;
	//	if (lightDepthValue < depthValue)
	//	{
	//		lightIntensity = saturate(dot(input.normal, input.lightPos));

	//		if (lightIntensity > 0.0f)
	//		{
	//			float3 combinecolor = clamp(dcolor + pcolor2 + scolor, 0, 1);
	//		}
	//	}
	//}
	float3 baseColor;

	float3 dc = diffTexture.Sample(filters[0], input.color.xy);
	baseColor = dc * combinecolor;
	float a = (diffTexture.Sample(filters[0], input.color.xy)).a;
	if (a < 0.5)
		discard;
	return float4(baseColor,a);// baseColor; // return a transition based on the detail alpha
}
