#include "common.hlsl"

// 疑似乱数生成関数（座標から一様乱数を作る）
float rand2D(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

float4 main(in PS_IN pin) : SV_Target
{
    float4 outDiffuse;

    if (Material.TextureEnable)
    {
        outDiffuse = g_Texture.Sample(g_SamplerState, pin.TexCoord);
        outDiffuse *= pin.Diffuse;
    }
    else
    {
        outDiffuse = pin.Diffuse;
    }

    // ノイズ値を生成（TexCoordやスクリーン座標を種にする）
    float noise = rand2D(pin.TexCoord * 100.0); // スケールで粒度調整
    
    // ノイズカラー（赤強め、緑青弱め）
    float3 noiseColor = float3(0.6, 0.3, 0.3);

    // ノイズを色に加算（例：グレースケールノイズ）
    outDiffuse.rgb += noiseColor * noise * 0.9;//ノイズ色*ノイズ*ノイズの強さ

    return outDiffuse;
}