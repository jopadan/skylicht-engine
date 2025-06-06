Texture2D uTexture : register(t0);
SamplerState uTextureSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uColorIntensity;
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 texColor = uTexture.Sample(uTextureSampler, input.tex0);	
	float3 color = lerp(float3(0.0, 0.0, 0.0), texColor.rgb, texColor.a);	
	float3 result = sRGB(input.color.rgb * color * input.color.a * uColorIntensity.rgb);
	return float4(result, 1.0);
}