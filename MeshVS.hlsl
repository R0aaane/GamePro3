
#include "Mesh.hlsli"

cbuffer cbuff : register(b0)
{
    matrix world;
    matrix uvMat;
}

cbuffer cbuff : register(b1)
{
    matrix viewProj;
}

VS_Output MeshVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VS_Output output;

    output.spos = mul(viewProj, mul(world, pos));
    output.uv   = mul(uvMat, float4(uv, 0.0f, 1.0f)).xy;

    return output;
}


