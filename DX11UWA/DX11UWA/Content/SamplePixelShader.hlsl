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

//static float3 DirectionalLight = { 0.0f,5.0f,0.0f };// : register(l0);
//static float3 PointLightPosition = { 0.0f,0.0f,-1.0f };// : register(l1);
//static float3 SpotLightPosition = { 0.0f,1.0f,0.8f };
//static float3 DLcolor = { 1.0f,1.0f,1.0f };// : register(c0);
//static float3 PLcolor = { 0.0f,0.0f,1.0f };// : register(c1);
//static float3 SLcolor = { 1.0f,0.0f,0.0f };
//static float lightradius = 2.0f;// : register(r0);
//
//static float3 conedir = { 0.5f,0.5f,0.5f };
//static float coneratio = 0.8f;

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
	//float bias;
	//float3 combinecolor;
	//float2 projectTexCoord;
	//float depthValue;
	//float lightDepthValue;
	//float lightIntensityD;
	//float lightIntensityP;
	//float lightIntensityS;

	float SPECULARINTENSITY = 1.0f;
	float SPECULARPOWER = 60.0f;

	float3 lightDirP = normalize(PointLightPosition.xyz - input.WorldPos.xyz);
	float3 LightDirS = normalize(SpotLightPosition.xyz - input.WorldPos.xyz);

	float dotD = clamp(dot(input.normal, normalize(DirectionalLight.xyz)), 0, 1);
	float dotP = clamp(dot(input.normal, lightDirP), 0, 1);
	float dotS = clamp(dot(-LightDirS, normalize(conedir.xyz)), 0, 1);

	float spotfactor = (dotS > coneratio.x) ? 1 : 0;
	spotfactor *= clamp(dot(LightDirS, normalize(input.normal)), 0, 1);

	float3 viewdir = normalize(input.CamWorldPos.xyz - input.WorldPos.xyz);
	float3 revectorD = reflect(normalize(DirectionalLight.xyz), normalize(input.normal));
	float3 revectorP = reflect(-lightDirP, normalize(input.normal));
	float3 revectorS = reflect(-LightDirS, normalize(input.normal));

	float intensityD = pow(dot(normalize(revectorD), viewdir), SPECULARPOWER);
	float intensityP = pow(dot(normalize(revectorP), viewdir), SPECULARPOWER);
	float intensityS = pow(dot(normalize(revectorS), viewdir), SPECULARPOWER);

	//float3 halfvectorD = normalize((-normalize(DirectionalLight.xyz)) + viewdir);
	//float3 halfvectorP = normalize((-lightDirP) + viewdir);
	//float3 halfvectorS = normalize((-LightDirS) + viewdir);
	//float intensityD = max(pow(clamp(dot(normalize(input.normal), normalize(halfvectorD)), 0, 1), SPECULARPOWER), 0);
	//float intensityP = max(pow(clamp(dot(normalize(input.normal), normalize(halfvectorP)), 0, 1), SPECULARPOWER), 0);
	//float intensityS = max(pow(clamp(dot(normalize(input.normal), normalize(halfvectorS)), 0, 1), SPECULARPOWER), 0);

	float3 dcolor = DLcolor.xyz *clamp((dotD + 0.2f), 0, 1);
	float3 pcolor = PLcolor.xyz *dotP;
	float3 scolor = spotfactor*SLcolor.xyz;

	float ATTENUATION = 1.0 - clamp((lightDirP / lightradius.x), 0, 1);

	float3 pcolor2 = pcolor * ATTENUATION;
	//float3 resultD = dcolor * SPECULARINTENSITY * intensityD;
	//float3 resultP = pcolor2 * SPECULARINTENSITY * intensityP;
	//float3 resultS = scolor * SPECULARINTENSITY * intensityS;
	float3 white = float3(1.0, 1.0, 1.0);
	float3 resultD = white * SPECULARINTENSITY * intensityD;
	float3 resultP = white * SPECULARINTENSITY * intensityP;
	float3 resultS = white * SPECULARINTENSITY * intensityS;

	//bias = 0.001f;
	//combinecolor = ( 0.0f,0.0f,0.0f );
	//projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	//projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
	//if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	//{
	//	depthValue = input.pos.z;
	//	lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
	//	lightDepthValue = lightDepthValue - bias;
	//	if (lightDepthValue < depthValue)
	//	{
	//		lightIntensityD = saturate(dot(input.normal, normalize(input.pos - input.WorldPos)));
	//		lightIntensityP = saturate(dot(input.normal, normalize(input.pos - input.WorldPos)));
	//		lightIntensityS = saturate(dot(input.normal, normalize(input.pos - input.WorldPos)));
	//		if (lightIntensityD > 0.0f)
	//		{
	//			combinecolor += dcolor * lightIntensityD;
	//		}
	//		if (lightIntensityP > 0.0f)
	//		{
	//			combinecolor += pcolor2 * lightIntensityP; 
	//		}
	//		if (lightIntensityS > 0.0f)
	//		{
	//			combinecolor += scolor * lightIntensitys;
	//		}
	//		combinecolor = saturate(combinecolor);
	//	}
	//}
	//float3 lightDirP = normalize(PointLightPosition.xyz - input.WorldPos.xyz);
	//float3 LightDirS = normalize(SpotLightPosition.xyz - input.WorldPos.xyz);
	//float dotD = clamp(dot(input.normal, normalize(DirectionalLight.xyz)), 0, 1);
	//float dotP = clamp(dot(input.normal, lightDirP), 0, 1);
	//float dotS = clamp(dot(-LightDirS, normalize(conedir.xyz)), 0, 1);
	//float spotfactor = (dotS > coneratio.x) ? 1 : 0;
	//spotfactor *= clamp(dot(LightDirS, normalize(input.normal)), 0, 1);
	//
	//float3 dcolor = DLcolor.xyz *clamp((dotD + 0.2f),0,1);
	//float3 pcolor = PLcolor.xyz *dotP;
	//float3 scolor = spotfactor*SLcolor.xyz;
	//				
	//float ATTENUATION = 1.0 - clamp((lightDirP / lightradius.x), 0, 1);
	//
	//float3 pcolor2 = pcolor * ATTENUATION;

	//float3 combinecolor = clamp(resultD + resultP + resultS, 0, 1);
	float3 combinecolor = clamp(dcolor + pcolor2 + scolor + resultD + resultP + resultS, 0, 1);
	
	float3 baseColor = diffTexture.Sample(filters[0], input.color.xy) *combinecolor;
	float a = (diffTexture.Sample(filters[0], input.color.xy)).a;
	if (a < 0.5)
		discard;

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

	return float4(baseColor,a);// baseColor; // return a transition based on the detail alpha
}
