Texture2D uTexColor : register(t0);
SamplerState uTexColorSampler : register(s0);

#if defined(AUTO_EXPOSURE)
Texture2D uTexLuminance : register(t1);
SamplerState uTexLuminanceSampler : register(s1);
#else
cbuffer cbPerFrame
{
	float2 uExposure;
};
#endif

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

#include "LibToneMapping.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 color = uTexColor.Sample(uTexColorSampler, input.tex0);

#if defined(AUTO_EXPOSURE)
	float avgLuminance = uTexLuminance.SampleLevel(uTexLuminanceSampler, input.tex0, 10.0).x;

#if defined(TONE_MAPPING)
	float target = 0.2;
	float threshold = 2.0;
#else
	float target = 0.3;
	float threshold = 2.0;
#endif

	float linearExposure = max((target / avgLuminance), 0.0001);

	// Function: exposure = exp(log(target/x)) see function graph for more detail
	float exposure = min(exp2(log2(linearExposure)), threshold);
#else
	float exposure = uExposure.x;
#endif

	return float4(linearRGB(exposure * color.rgb), color.a);
}