#pragma once
#include "Object.h"
#include "Buller.h"

class M_Gun : public Object
{
private:
	// 弾情報
	std::vector<std::unique_ptr<Bullet>> m_bullet;

	Vector3 forward;//前方ベクトルを取得するための変数
	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width;
	float Height;
	float Depth;
public:
	M_Gun();
	~M_Gun();
	void Init() override;
	void Update(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;
};