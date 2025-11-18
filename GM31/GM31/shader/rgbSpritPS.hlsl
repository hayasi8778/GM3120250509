#include "common.hlsl"

// シェーダーリソース
Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

//// 色収差用パラメータ
//cbuffer AberrationCB : register(b0)
//{
//    float2 offsetR; // 赤チャンネルのUVオフセット
//    float2 offsetG; // 緑チャンネルのUVオフセット
//    float2 offsetB; // 青チャンネルのUVオフセット
//};


float4 main(PS_IN pin) : SV_TARGET
{
 //   float2 offsetR = { 2.0f, 0 }; // 赤チャンネルのUVオフセット
 //   float2 offsetG = { 2.0f, 0 }; // 緑チャンネルのUVオフセット
 //   float2 offsetB = { 2.0f, 0 }; // 青チャンネルのUVオフセット
    
	//// 各チャンネルをオフセットしてサンプル
 //   float4 colR = g_Texture.Sample(g_Sampler, pin.TexCoord + offsetR);
 //   float4 colG = g_Texture.Sample(g_Sampler, pin.TexCoord + offsetG);
 //   float4 colB = g_Texture.Sample(g_Sampler, pin.TexCoord + offsetB);

 //   // チャンネルごとに組み合わせ
 //   float4 result;
 //   result.r = colR.r;
 //   result.g = colG.g;
 //   result.b = colB.b;
 //   // alpha は各チャンネルの平均を使うか、単一のサンプルから流用
 //   result.a = (colR.a + colG.a + colB.a) / 3.0;

 //   // 頂点カラー乗算
 //   return result * pin.Diffuse;
    
    float2 uv = pin.TexCoord;
    float2 center = float2(0.5, 0.5);
    float2 dir = uv - center;
    float strength = 0.2; // 色収差の強さ
    float blendAmt = 1.0; // 1.0で全収差、0.5で半分

    // チャンネルごとのUVオフセット
    float2 oR = uv + dir * strength;
    float2 oG = uv + dir * (strength * 0.5);
    float2 oB = uv - dir * strength;

    // サンプル
    float4 colR = g_Texture.Sample(g_Sampler, oR);
    float4 colG = g_Texture.Sample(g_Sampler, oG);
    float4 colB = g_Texture.Sample(g_Sampler, oB);

    // 合成
    float4 chroma;
    chroma.r = colR.r;
    chroma.g = colG.g;
    chroma.b = colB.b;
    chroma.a = (colR.a + colG.a + colB.a) / 3.0;

    // 元画像とブレンドして出力
    float4 orig = g_Texture.Sample(g_Sampler, uv);
    float4 result = lerp(orig, chroma, blendAmt);

    return result * pin.Diffuse;


}