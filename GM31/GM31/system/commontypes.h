#pragma once
#include	<wrl/client.h>
#include	<cstdint>
#include	<numbers>
#include	<SimpleMath.h>

// ��{�^�̃G�C���A�X
using Vector4 = DirectX::SimpleMath::Vector4;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector2 = DirectX::SimpleMath::Vector2;

using Matrix4x4 = DirectX::SimpleMath::Matrix;

using Color = DirectX::SimpleMath::Color;

using Quaternion = DirectX::SimpleMath::Quaternion;

using Microsoft::WRL::ComPtr;

//"C++20����̕��Ȃ̂Ńv���W�F�N�g��C++20�ɂ���"
constexpr float PI = std::numbers::pi_v<float>;
