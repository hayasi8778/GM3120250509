#include "common.hlsl"

cbuffer MatrixBuffer : register(b6)
{
    matrix LightViewProj; // ライトのViewProjection行列
}

cbuffer MaterialBuffer : register(b7)
{
    float4 MaterialDiffuse;
}

struct PS_IN_SHADOW
{
    float4 Position : SV_POSITION; // スクリーン座標
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float4 WorldPos : TEXCOORD1; // ワールド座標
    float4 ShadowPos : TEXCOORD2; // ライト視点の位置（シャドウマップ用）
};

PS_IN_SHADOW main(VS_IN In)
{
    PS_IN_SHADOW Out;

    matrix wvp = mul(World, View);
    wvp = mul(wvp, Projection);

    // 各種座標変換
    Out.Position = mul(In.Position, wvp);
    Out.WorldPos = mul(In.Position, World);
    Out.ShadowPos = mul(In.Position, LightViewProj);
    Out.TexCoord = In.TexCoord;

    Out.Diffuse = In.Diffuse * MaterialDiffuse;

    return Out;
}