#pragma once
#include <iostream>
#include <DirectXMath.h>
#include <SimpleMath.h>   // DirectX::SimpleMath::Matrix

using namespace DirectX;
using namespace DirectX::SimpleMath;

//どこかに共通ユーティリティ関数として書いておくと便利
void DumpMatrix(char const* name, Matrix const& mat)
{
    // 1) SimpleMath::Matrix は内部的に XMMATRIX/ XMFLOAT4X4 を持っているので
    //    そのまま XMMATRIX にキャストできます
    XMMATRIX xm = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4 const*>(&mat));

    // 2) XMFLOAT4X4 構造に格納
    XMFLOAT4X4 f;
    XMStoreFloat4x4(&f, xm);

    // 3) 標準出力またはデバッガで確認
    std::cout << name << " = {\n";
    std::cout << "  " << f._11 << ", " << f._12 << ", " << f._13 << ", " << f._14 << "\n";
    std::cout << "  " << f._21 << ", " << f._22 << ", " << f._23 << ", " << f._24 << "\n";
    std::cout << "  " << f._31 << ", " << f._32 << ", " << f._33 << ", " << f._34 << "\n";
    std::cout << "  " << f._41 << ", " << f._42 << ", " << f._43 << ", " << f._44 << "\n";
    std::cout << "}\n";
}
