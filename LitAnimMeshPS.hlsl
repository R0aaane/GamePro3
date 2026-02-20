
#include "LitAnimMesh.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 LitAnimMeshPS(VS_Output input) : SV_TARGET
{
    // 法線の正規化
    float3 n = normalize(input.normW);
    
    // 平行光源
    float3 lambert = paraLightCol * max(0.0f, -dot(n, paraLightDir));
    
    float3 r = reflect(paraLightDir, n);
    float3 e = normalize(eye - input.posW);
    float3 specular = paraLightCol * pow(max(0.0f, dot(r, e)), spow);
    
    // ポイントライト
    for (int i = 0; i < PointLightNum; ++i)
    {
        float3 v = pointLight[i].pos - input.posW;
        float3 ray = normalize(v);
        float len = length(v);
        float affect = pow(max(0.0f, 1.0f - len / pointLight[i].radius), 2.0f);
        r = reflect(-ray, n);
    
        lambert += affect * pointLight[i].col * max(0.0f, dot(n, ray));
        specular += affect * pointLight[i].col * pow(max(0.0f, dot(r, e)), spow);
    }
    
    // スポットライト
    for (i = 0; i < SpotLightNum; ++i)
    {
        float3 v = spotLight[i].pos - input.posW;
        float len = length(v);
        float3 ray = normalize(v);
        float rangle = radians(spotLight[i].angle);
        float affectD = pow(max(0.0f, 1.0f - len / spotLight[i].radius), 2.0f);
        float affectR =
            pow(max(0.0f, 1.0f - acos(-dot(ray, spotLight[i].dir)) / rangle), 0.5f);
        float affect = affectD * affectR;
        r = reflect(-ray, n);
    
        lambert += affect * spotLight[i].col * max(0.0f, dot(n, ray));
        specular += affect * spotLight[i].col * pow(max(0.0f, dot(r, e)), spow);
    }
    
    // 環境光
    float3 ambient = amb * ambientLight;
    
    // 反射光強度
    lambert *= diff;
    specular *= spec;
    float3 brightness = max(lambert + specular, ambient);
    
    // 出力
    float4 col = tex.Sample(smp, input.uv);
    return float4(col.rgb * brightness, 1.0f);
}

