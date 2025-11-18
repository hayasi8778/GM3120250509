#pragma once
#include "Object.h"
#include "Buller.h"

class M_Gun : public Object
{
private:
	//玉出る方向にレイ出す

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	int bulletnum = 0;

	Bullet m_bullets[5];

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width;
	float Height;
	float Depth;

	const int ATK = 1;
public:
	M_Gun();
	~M_Gun();
	void Init() override;
	void Update(uint64_t deltatime) override;
	void LateUpdate(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	void Reset();
	int GetShaderNum() override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB_Bullet(int num);
	void SetCollision_Bullet(int, bool);

	int Damage_Bullet() { return ATK; }

	const int BulletMaxnum = 5;
};