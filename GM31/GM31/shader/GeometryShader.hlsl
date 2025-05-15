#include "common.hlsl"

[maxvertexcount(6)]
void main(line PS_IN input[2], inout TriangleStream<PS_IN> OutputStream)
{
    PS_IN output;
    float thickness = 0.3f; // ���̑����A�K�X����

    // ���̕������v�Z
    float2 dir = normalize(input[1].Position.xy - input[0].Position.xy);

    // ���K�����ꂽ�����ɒ�������x�N�g�����v�Z�i���̕��̂��߁j
    float2 normal = float2(-dir.y, dir.x) * thickness;

    // �l�p�`�̒��_���v�Z�i2�̎O�p�`���`���j
    float4 pos1 = input[0].Position + float4(normal, 0.0, 0.0);
    float4 pos2 = input[0].Position - float4(normal, 0.0, 0.0);
    float4 pos3 = input[1].Position + float4(normal, 0.0, 0.0);
    float4 pos4 = input[1].Position - float4(normal, 0.0, 0.0);

    // �ŏ��̎O�p�`
    output.Position = pos1;
    output.Diffuse = input[0].Diffuse;
    output.TexCoord.xy = 0.0f;
    OutputStream.Append(output);

    output.Position = pos2;
    output.Diffuse = input[0].Diffuse;
    output.TexCoord.xy = 0.0f;
    OutputStream.Append(output);
    
    output.Position = pos3;
    output.Diffuse = input[0].Diffuse;
    output.TexCoord.xy = 0.0f;
    OutputStream.Append(output);

    // ��Ԗڂ̎O�p�`
    output.Position = pos2;
    output.Diffuse = input[1].Diffuse;
    output.TexCoord.xy = 0.0f;
    OutputStream.Append(output);
    
    output.Position = pos4;
    output.Diffuse = input[1].Diffuse;
    output.TexCoord.xy = 0.0f;
    OutputStream.Append(output);

    output.Position = pos3;
    output.Diffuse = input[1].Diffuse;
    output.TexCoord.xy = 0.0f;
    OutputStream.Append(output);
    
    OutputStream.RestartStrip();
}