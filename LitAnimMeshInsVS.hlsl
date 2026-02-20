
#include "LitAnimMesh.hlsli"

VS_Output LitAnimMeshInsVS(float4 pos : POSITION, float2 uv : TEXCOORD,
	float4 norm : NORMAL, min16uint4 boneno : BONENO, float4 weight : WEIGHT,
    uint instanceID : SV_InstanceID)
{
    VS_Output output;
    
    float4 p = mul(pose[boneno[0]], mul(igpose[boneno[0]], pos)) * weight[0];
    p += mul(pose[boneno[1]], mul(igpose[boneno[1]], pos)) * weight[1];
    p += mul(pose[boneno[2]], mul(igpose[boneno[2]], pos)) * weight[2];
    p += mul(pose[boneno[3]], mul(igpose[boneno[3]], pos)) * weight[3];
    
    float4 normU = float4(norm.xyz, 0.0f);
    float4 n = mul(pose[boneno[0]], mul(igpose[boneno[0]], normU)) * weight[0];
    n += mul(pose[boneno[1]], mul(igpose[boneno[1]], normU)) * weight[1];
    n += mul(pose[boneno[2]], mul(igpose[boneno[2]], normU)) * weight[2];
    n += mul(pose[boneno[3]], mul(igpose[boneno[3]], normU)) * weight[3];
	
    output.posW = mul(world, p).xyz;
    if (instanceID == 1)
    {
        output.posW += float3(0.0f, 1.5f, 0.0f);
    }
    else if (instanceID == 2)
    {
        output.posW += float3(0.0f, -1.5f, 0.0f);
    }
    output.spos = mul(viewProj, float4(output.posW, 1.0f));
    output.uv = uv;
    float4 wn = mul(world, float4(n.xyz, 0.0f));
    output.normW = normalize(float3(wn.x / scale.x, wn.y / scale.y, wn.z / scale.z));
    
    return output;
}






