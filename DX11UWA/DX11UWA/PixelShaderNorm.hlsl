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

texture2D diffTexture : register(t0);

texture2D normTexture : register(t1);

texture2D specTexture : register(t2);

SamplerState filters[3] : register(s0);

static float3 DirectionalLight = { 0.0f,5.0f,0.0f };// : register(l0);
static float3 PointLightPosition = { 0.0f,0.0f,-1.0f };// : register(l1);
static float3 SpotLightPosition = { 0.0f,1.0f,0.8f };
static float3 DLcolor = { 1.0f,1.0f,1.0f };// : register(c0);
static float3 PLcolor = { 0.0f,0.0f,1.0f };// : register(c1);
static float3 SLcolor = { 1.0f,0.0f,0.0f };
static float lightradius = 2.0f;// : register(r0);

static float3 conedir = { 0.5f,0.5f,0.5f };
static float coneratio = 0.8f;

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 nc = normTexture.Sample(filters[1], input.color.xy);
	float3 newNormal = (nc*2.0f) - 1.0f;

	float3 normalo = normalize(input.normal);
	float3 tangento = normalize(input.tangent.xyz);
	float3 bitangento = normalize(input.bitangent.xyz);

	float3x3 TBN;
	TBN[0] = tangento.xyz;
	TBN[1] = bitangento.xyz;
	TBN[2] = normalo.xyz;

	newNormal = mul(newNormal, TBN);

	float3 lightDirP = normalize(PointLightPosition - newNormal);//input.WorldPos);
	float3 LightDirS = normalize(SpotLightPosition - newNormal);//input.WorldPos);

	//float dotD = clamp(dot(input.normal, DirectionalLight), 0, 1);
	//float dotP = clamp(dot(input.normal, lightDirP), 0, 1);
	float dotD = clamp(dot(newNormal, DirectionalLight), 0, 1);
	float dotP = clamp(dot(newNormal, lightDirP), 0, 1);
	float dotS = clamp(dot(LightDirS, conedir), 0, 1);

	float spotfactor = (dotS > coneratio) ? 1 : 0;

	float3 dcolor = DLcolor *clamp((dotD + 0.2f),0,1);
	float3 pcolor = PLcolor *dotP;
	float3 scolor = spotfactor*dotS*SLcolor;

	float ATTENUATION = 1.0 - clamp((lightDirP / lightradius), 0, 1);

	float3 pcolor2 = pcolor * ATTENUATION;
	float3 combinecolor = clamp(dcolor + pcolor2 + scolor, 0, 1);

	float3 baseColor;
	float3 dc = diffTexture.Sample(filters[0], input.color.xy);
	//float3 nc = normTexture.Sample(filters[1], input.color.xy);
	//float3 sc = specTexture.Sample(filters[2], input.color.xy);
	//float3 basec = saturate(dc*nc);//dc*0.5*sc*0.5
	baseColor = dc * combinecolor;
	float a = (diffTexture.Sample(filters[0], input.color.xy)).a;
	if (a < 0.5)
		discard;
	return float4(baseColor,a);// baseColor; // return a transition based on the detail alpha
}
