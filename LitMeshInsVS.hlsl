
#include "LitMesh.hlsli"

VS_Output LitMeshInsVS(float4 pos : POSITION, float2 uv : TEXCOORD,
	float4 norm : NORMAL, uint instanceID : SV_InstanceID)
{
    VS_Output output;
	
    output.posW = mul(world, pos).xyz;
    if (instanceID == 1)
    {
        output.posW += float3(0.0f, 1.5f, 0.0f);
    }
    else if (instanceID == 2)
    {
        output.posW += float3(0.0f, -1.5f, 0.0f);
    }
    output.spos = mul(viewProj, float4(output.posW, 1.0f));
    output.uv = mul(uvMat, float4(uv, 0.0f, 1.0f)).xy;
    float4 wn = mul(world, float4(norm.xyz, 0.0f));
    output.normW = normalize(float3(wn.x / scale.x, wn.y / scale.y, wn.z / scale.z));
    
    return output;
}






