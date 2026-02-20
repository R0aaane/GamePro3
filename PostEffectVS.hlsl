
#include "PostEffect.hlsli"

VS_OUTPUT PostEffectVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VS_OUTPUT output;
    
    output.pos = pos;
    output.uv  = uv;
    
    return output;
}



