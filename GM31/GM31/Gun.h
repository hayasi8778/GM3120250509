#pragma once
#include "Object.h"
#include "Buller.h"

class M_Gun : public Object
{
private:

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	// 弾情報
	std::vector<std::unique_ptr<Bullet>> m_bullet;

	int bulletnum = 0;

	Bullet m_bullets[5];

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
	GM31::GE::Collision::BoundingBoxOBB GetOBB_Bullet(int num);
	void SetCollision_Bullet(int, bool);

	const int BulletMaxnum = 5;
};