
#include "NormalMap.hlsli"

VS_Output NormalMapVS(float4 pos : POSITION, float2 uv : TEXCOORD,
	float4 norm : NORMAL, float4 tangent : TANGENT)
{
    VS_Output output;
	
    output.posW = mul(world, pos).xyz;
    output.spos = mul(viewProj, float4(output.posW, 1.0f));
    output.uv = mul(uvMat, float4(uv, 0.0f, 1.0f)).xy;
    float4 wn = mul(world, float4(norm.xyz, 0.0f));
    output.normW = normalize(float3(wn.x / scale.x, wn.y / scale.y, wn.z / scale.z));
    
    float4 tn = mul(world, float4(tangent.xyz, 0.0f));
    output.tangentW = normalize(float3(tn.x / scale.x, tn.y / scale.y, tn.z / scale.z));
    output.binormalW = normalize(cross(output.normW, output.tangentW));
    
    return output;
}






