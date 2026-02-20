
#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp      : register(s0);

float4 PostEffectPS(VS_OUTPUT input) : SV_Target
{
    return tex.Sample(smp, input.uv);
}

