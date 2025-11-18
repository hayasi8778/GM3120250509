#include "common.hlsl"

// シェーダーリソース（サンプラは一つにする）
Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

Texture2D ShadowMap : register(t2); // 深度テクスチャ SRV（r 成分に深度）

cbuffer LightBuffer : register(b8)
{
    float4 LightDirection;
    float4 LightColor;
    float ShadowBias;
    float ShadowSoftness;
    float2 Padding;
};

struct PS_IN_SHADOW
{
    float4 Position : SV_POSITION;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float4 WorldPos : TEXCOORD1;
    float4 ShadowPos : TEXCOORD2;
};

float4 main(PS_IN_SHADOW In) : SV_TARGET
{
    float4 baseColor = g_Texture.Sample(g_Sampler, In.TexCoord) * In.Diffuse;
    
    //return g_Texture.Sample(g_Sampler, In.TexCoord); //ベースカラーを返す

    // ライト視点からの位置 → NDC へ → [0,1] 空間へ
    float3 shadowCoord = In.ShadowPos.xyz / In.ShadowPos.w;
    shadowCoord = shadowCoord * 0.5f + 0.5f;

    // カメラ外は影なし
    if (shadowCoord.x < 0 || shadowCoord.x > 1 || shadowCoord.y < 0 || shadowCoord.y > 1)
        return baseColor;

    // 手動PCF（サンプルを取り深度と比較して平均化）
    const int sampleCount = 4;
    float2 offsets[sampleCount] =
    {
        float2(-1, -1), float2(1, -1),
        float2(-1, 1), float2(1, 1)
    };

    float shadow = 0.0f;
    float cmpDepth = shadowCoord.z - ShadowBias;

    // サンプラのフィルタが LINEAR の場合、Sample は補間済み深度を返すのでソフト影に寄与します
    [unroll]
    for (int i = 0; i < sampleCount; ++i)
    {
        float2 uv = shadowCoord.xy + offsets[i] * ShadowSoftness;
        // 深度を通常サンプラで取得（r 成分）
        float sampledDepth = ShadowMap.Sample(g_Sampler, uv).r;
        // sampledDepth が小さい（シーン中のピクセルがライトに近い）なら明るい
        // ここでは shadow = 1 => 明るい, 0 => 影 とする
        shadow += sampledDepth >= cmpDepth ? 1.0f : 0.0f;
    }
    shadow /= sampleCount;

    float shadowFactor = shadow;

    // 簡易拡散光（法線を上向き固定）
    float3 lightDir = normalize(-LightDirection.xyz);
    float NdotL = saturate(dot(lightDir, float3(0, 1, 0)));

    float3 color = baseColor.rgb * LightColor.rgb * NdotL * shadowFactor;
    
    color = baseColor.rgb;

    
    return float4(color, baseColor.a);
}