
#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PostEffectInvertPS(VS_OUTPUT input) : SV_Target
{
    float3 color = tex.Sample(smp, input.uv).rgb;
    
    return float4(1.0f - color, 1.0f);
}

