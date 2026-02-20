
// 頂点シェーダからの出力、ピクセルシェーダの入力
struct VS_Output
{
    float4 spos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

