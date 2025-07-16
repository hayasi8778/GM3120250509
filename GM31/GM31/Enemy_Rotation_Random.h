#pragma once
#include <cstdint>
#include <algorithm>  // std::swap

class XorShift32
{
    uint32_t state;

public:
    explicit XorShift32(uint32_t seed = 2463534242u)
        : state(seed)
    {
    }

    // 乱数本体：0 ～ UINT32_MAX の uint32 を返す
    uint32_t Next()
    {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }

    // 動的範囲対応バージョン
    float RandF(float min, float max)
    {
        if (min > max)
            std::swap(min, max);

        // [0.0, 1.0] の float に正規化
        float normalized = static_cast<float>(Next()) / static_cast<float>(UINT32_MAX);
        return min + normalized * (max - min);
    }

    // デフォルト範囲版（従来通り 0～1.5）
    float RandF()
    {
        return RandF(0.0f, 1.5f);
    }
};

// グローバルかつスレッドローカルでも良い
static XorShift32 xs(12345);

// 利便性ラッパー
inline float RandomFloat(float min, float max)
{
    return xs.RandF(min, max);
}
