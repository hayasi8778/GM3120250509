#pragma once
#include <random>
#include <SimpleMath.h> // DirectXTK SimpleMath

using namespace DirectX::SimpleMath;

//ÇªÇÒÇ»Ç…ê∏ìxçÇÇ¢ÇÌÇØÇ∂Ç·Ç»Ç¢ÇÁÇµÇ¢ÇÃÇ≈â¸ó«ÇµÇƒÇ‡Ç¢Ç¢Ç©Ç‡
struct RandomGen {
    std::mt19937 rng;

    RandomGen(uint32_t seed = std::random_device{}()) : rng(seed) {}

    float UniformFloat(float a = 0.0f, float b = 1.0f) {
        std::uniform_real_distribution<float> dist(a, b);
        return dist(rng);
    }

    Vector3 RandomVector3(float min = 0.0f, float max = 1.0f) {
        return Vector3(UniformFloat(min, max),
            UniformFloat(min, max),
            UniformFloat(min, max));
    }

    int UniformInt(int a, int b) {
        std::uniform_int_distribution<int> dist(a, b);
        return dist(rng);
    }
};
