#pragma once
#include "Object.h"

class Bullet : public Object
{
private:
	Vector3 forward;//前方ベクトルを取得するための変数
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Dispose() override;
};