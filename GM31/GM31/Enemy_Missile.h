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

	//弾をよける動き作るために一番近い弾の位置とその向きを取得する
	Vector3 CurentBulletpos = { 0,0,0 };
	Vector3 CurentBulletrot = { 0,0,0 };

	const int MaxHP = 5;
	int HP = MaxHP;

	const int BulletMaxnum = 5;

	int Bulletnum = 0;

	E_Missile e_missiles[5];

	float cooltime = 1000;

	M_Player* player = nullptr;//プレイヤー

	bool FIRE = true;

	float Invincibility_time = 0;//無敵時間
	bool collision_hit = false;
	bool interception = false; //迎撃範囲に入っているか
	float interception_time = 0;

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
	void Reset();
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;
	GM31::GE::Collision::BoundingSphere GetShere();
	void CreateBullet();

	int GetHP() { return HP; }
	int GetMaxHP() { return MaxHP; }
	void SetPlayer(M_Player* pl);
	GM31::GE::Collision::BoundingBoxOBB GetOBB_Bullet(int bulletnum);

	Vector3 GetCenter() { return m_Position - Forward_vec * 1.5f; }//当たり判定の中心

	void SetCollision(bool col);
	void SetCollision_Bullet(int,bool);
	void SetInterception(bool inter) { interception = inter; }
};