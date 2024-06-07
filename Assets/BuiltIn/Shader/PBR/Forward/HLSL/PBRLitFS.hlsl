// File Generated by Assets/BuildShader.py - source: [PBRLitFS.d.hlsl : EMISSIVE]
Texture2D uTexAlbedo : register(t0);
SamplerState uTexAlbedoSampler : register(s0);
Texture2D uTexNormalMap : register(t1);
SamplerState uTexNormalMapSampler : register(s1);
Texture2D uTexRMA : register(t2);
SamplerState uTexRMASampler : register(s2);
TextureCube uTexReflect : register(t3);
SamplerState uTexReflectSampler : register(s3);
Texture2D uTexBRDF : register(t4);
SamplerState uTexBRDFSampler : register(s4);
Texture2D uTexEmissive : register(t5);
SamplerState uTexEmissiveSampler : register(s5);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
};
cbuffer cbPerFrame
{
	float4 uLightColor;
	float4 uColor;
	float4 uSHConst[4];
};
static const float gamma = 2.2;
static const float invGamma = 1.0 / 2.2;
float3 sRGB(float3 color)
{
	return pow(color, gamma);
}
float3 linearRGB(float3 color)
{
	return pow(color, invGamma);
}
static const float PI = 3.1415926;
float3 fresnelSchlick(float3 V, float3 H, float3 F0)
{
	float VdotH = max(dot(V, H), 0.001);
	return F0 + (1.0 - F0) * pow((1.0 - VdotH), 5.0);
}
float gaSchlickG1(float3 N, float3 V, float k)
{
	float NdotV = max(dot(N, V), 0.001);
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return nom / denom;
}
float schlickGGX(float3 N, float3 V, float3 L, float roughness)
{
	float k = 0.001 + (roughness + 1.0) * (roughness + 1.0) / 8.0;
	return gaSchlickG1(N, L, k) * gaSchlickG1(N, V, k);
}
float ndfGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float nom = a2;
	float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
	denom = PI * denom * denom;
	return nom / denom;
}
float3 computeLightContribution(float3 N, float3 L, float3 V, float3 F0, float3 lambert, float3 lightColor, float VdotN, float roughness, float metalness)
{
	float3 H = normalize(L + V);
	float D = ndfGGX(N, H, roughness);
	float G = schlickGGX(N, V, L, roughness);
	float3 F = fresnelSchlick(V, H, F0);
	float LdotN = max(dot(N, L), 0.0);
	float3 specularBRDF = (D * F * G) / (4.0 * VdotN * LdotN + 0.001);
	float3 kd = lerp(float3(1.0, 1.0, 1.0) - F, float3(0.0, 0.0, 0.0), metalness);
	float3 BRDF = kd * lambert + specularBRDF;
	return BRDF * lightColor * LdotN;
}
float4 main(PS_INPUT input) : SV_TARGET
{
	float4 albedoMap = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0) * uColor;
	float3 rmaMap = uTexRMA.Sample(uTexRMASampler, input.tex0).xyz;
	float3 emissiveMap = uTexEmissive.Sample(uTexEmissiveSampler, input.tex0).rgb;
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = normalize(mul(localCoords, rotation));
	n = normalize(n);
	float roughness = rmaMap.r;
	float metalness = rmaMap.g;
	float ao = rmaMap.b;
	float3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	ambientLighting = sRGB(ambientLighting * 0.9);
	float3 albedo = sRGB(albedoMap.rgb);
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metalness);
	float VdotN = max(dot(input.worldViewDir, n), 0.0);
	float3 lambert = albedo / PI;
	float3 lightContribution = computeLightContribution(n, input.worldLightDir, input.worldViewDir, F0, lambert, sRGB(uLightColor.rgb), VdotN, roughness, metalness);
	float3 F = fresnelSchlick(input.worldViewDir, n, F0);
	float3 kd = lerp(float3(1.0, 1.0, 1.0) - F, float3(0.0, 0.0, 0.0), metalness);
	float3 indirectDiffuse = ambientLighting * albedo;
	float3 reflection = -normalize(reflect(input.worldViewDir, n));
	float3 prefilteredColor = sRGB(uTexReflect.SampleLevel(uTexReflectSampler, reflection, roughness * 8).xyz);
	float2 envBRDF = uTexBRDF.Sample(uTexBRDFSampler, float2(VdotN, roughness)).rg;
	F = F0 * envBRDF.x + envBRDF.y;
	float3 indirectSpecular = prefilteredColor * F;
	float3 indirectLight = (kd * indirectDiffuse + indirectSpecular);
	lightContribution += sRGB(emissiveMap);
	return float4((lightContribution + indirectLight) * ao, albedoMap.a);
}