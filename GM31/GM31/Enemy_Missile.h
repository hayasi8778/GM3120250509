#pragma once
#include "Object.h"
#include "E_Missile.h"
#include "Player_Mec.h"

class Enemy_Missile : public Object
{
private:

	int HP = 5;

	// 弾情報
	std::vector<std::unique_ptr<E_Missile>> e_missile;

	E_Missile e_missiles[20];

	float cooltime = 1000;

	M_Player* player = nullptr;//プレイヤー

	bool FIRE = false;

	bool collision_hit = false;

	Vector3 forward;//前方ベクトルを取得するための変数

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width;
	float Height;
	float Depth;
public:
	Enemy_Missile();
	~Enemy_Missile();
	void Init() override;
	void Init(M_Player*);
	void Update(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;
	void CreateBullet();

	void SetPlayer(M_Player* pl);
	GM31::GE::Collision::BoundingBoxOBB GetOBB_Bullet(int bulletnum);
	void SetCollision(bool col) { if(col) collision_hit = col; }
};