#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

// モノクロ変換ピクセルシェーダー
float4 main(PS_IN In) : SV_Target
{
    // 1) テクスチャをサンプリングし、頂点カラーと乗算
    float4 color = g_Texture.Sample(g_Sampler, In.TexCoord) * In.Diffuse;

    // 2) 輝度（Luminance）を計算（Rec.709 準拠の重み付け）
    float lum = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));

     //return float4(1, 1, 0, 1);
    // 3) RGB に同じ値をセットしてグレースケール化、αは元のまま
    return float4(lum, lum, lum, color.a);
}
