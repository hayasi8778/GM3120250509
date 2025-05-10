#pragma once
#include	<wrl/client.h>
#include	<cstdint>
#include	<numbers>
#include	<SimpleMath.h>

// 基本型のエイリアス
using Vector4 = DirectX::SimpleMath::Vector4;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector2 = DirectX::SimpleMath::Vector2;

using Matrix4x4 = DirectX::SimpleMath::Matrix;

using Color = DirectX::SimpleMath::Color;

using Quaternion = DirectX::SimpleMath::Quaternion;

using Microsoft::WRL::ComPtr;

//"C++20限定の物なのでプロジェクトをC++20にする"
constexpr float PI = std::numbers::pi_v<float>;
