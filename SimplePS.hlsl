
#include "Simple.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState      smp : register(s0);

float4 SimplePS(VS_Output input) : SV_TARGET
{
	//return tex.Sample(smp, input.uv) * input.color;
	
    float4 color = tex.Sample(smp, input.uv);
    color.rgb *= color.a;
    return color * input.color;

}


