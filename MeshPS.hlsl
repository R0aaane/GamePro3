
#include "Mesh.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState      smp : register(s0);

float4 MeshPS(VS_Output input) : SV_TARGET
{
    //return tex.Sample(smp, input.uv);
    
    float4 color = tex.Sample(smp, input.uv);
    color.rgb *= color.a;
    return color;

}


