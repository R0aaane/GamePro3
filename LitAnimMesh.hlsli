
// 頂点シェーダからの出力、ピクセルシェーダの入力
struct VS_Output
{
    float4 spos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 posW : WPOSITION;
    float3 normW : WNORMAL;
};

// 定数バッファ
cbuffer cbuff : register(b0)
{
    matrix world;
    float3 scale;
    float3 diff;
    float3 spec;
    float spow;
    float3 amb;
    matrix igpose[256];
    matrix pose[256];
}

struct PointLight
{
    float3 pos;
    float3 col;
    float radius;
};
static const int PointLightNum = 10;

struct SpotLight
{
    float3 pos;
    float radius;
    float3 dir;
    float angle;
    float3 col;
};
static const int SpotLightNum = 3;

cbuffer cbuff : register(b1)
{
    matrix viewProj;
    float3 eye;
    float3 paraLightDir;
    float3 paraLightCol;
    PointLight pointLight[PointLightNum];
    SpotLight spotLight[SpotLightNum];
    float3 ambientLight;
}







