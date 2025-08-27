#pragma once
#include "Object.h"
#include "E_Missile.h"
#include "Player_Mec.h"

class Enemy_Missile : public Object
{
private:

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	const int MaxHP = 35;
	int HP = MaxHP;

	// 弾情報
	std::vector<std::unique_ptr<E_Missile>> e_missile;

	const int BulletMaxnum = 5;

	int Bulletnum = 0;

	E_Missile e_missiles[5];

	float cooltime = 1000;

	M_Player* player = nullptr;//プレイヤー

	bool FIRE = false;

	float Invincibility_time = 0;//無敵時間
	bool collision_hit = false;
	bool interception = false; //迎撃範囲に入っているか

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	std::unique_ptr<Sphere> m_interceptionSphere; //範囲内に入った弾を迎撃するシステム作りたい
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

	int GetHP() { return HP; }
	int GetMaxHP() { return MaxHP; }
	void SetPlayer(M_Player* pl);
	GM31::GE::Collision::BoundingBoxOBB GetOBB_Bullet(int bulletnum);
	void SetCollision(bool col);
	void SetCollision_Bullet(int,bool);
	void SetInterception(bool inter) { interception = inter; }
};