#pragma once
#include "Object.h"
#include "E_Missile.h"
#include "E_Beam.h"
#include "Player_Mec.h"

//階層化されたパーツ
#include "Enemy_Head.h"
#include "Enemy_LeftArm.h"
#include "Enemy_RightArm.h"
#include "Enemy_LeftFeet.h"
#include "Enemy_RightFeet.h"

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

	const int ATK_Bullet = 20;
	const int ATK_Beam = 1;

	const int BulletMaxnum = 25;

	int Bulletnum = 0;

	E_Missile e_missiles[25];

	E_Beam e_beam;

	Vector3 beamsize = { 1,1,1 };//ビームの大きさ
	Vector3 Maxbeamsize = { 25,25,100 };

	float cooltime = 0;
	//発射間隔
	float FireRate = 1000.0f;

	M_Player* player = nullptr;//プレイヤー

	bool FIRE = true;//弾の発射フラグ
	bool FIRE_BEAM = false;//ビームの照射フラグ
	float beam_time = 0;//ビームの照射

	float Invincibility_time = 0;//無敵時間
	bool collision_hit = false;
	bool Avoidance = false; //迎撃範囲に入っているか
	float Avoidance_Cooltime = 0;

	Vector3 AvoidanceVec = { 0,0,0 };//回避ベクトル
	float AvoidancePowor = 0;//回避の速度

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	std::unique_ptr<Sphere> m_interceptionSphere; //範囲内に入った弾を迎撃するシステム作りたい
	float Width;
	float Height;
	float Depth;

	//部位
	Enemy_Head Head;
	Enemy_LeftArm Leftarm;
	Enemy_RightArm Rightarm;
	Enemy_LeftFeet Leftfeet;
	Enemy_RightFeet Rightfeet;
public:
	Enemy_Missile();
	~Enemy_Missile();
	void Init() override;
	void Init(M_Player*);
	void Update(uint64_t deltatime) override;
	void LateUpdate(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	void Reset();
	int GetShaderNum() override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;
	GM31::GE::Collision::BoundingSphere GetShere();
	void CreateBullet();

	void Move();
	void Move(Vector3);
	void Timer(uint64_t);
	void Shot_Rule(uint64_t);
	void Shot(uint64_t);
	void FullBurst(uint64_t);
	//射撃フラグの反転
	void ReturnFire() {
		if (FIRE)	FIRE = false;
		else	FIRE = true;
	}
	bool GetFIRE() { return FIRE; }

	bool Collision_EN(GM31::GE::Collision::BoundingBoxOBB colobb);

	void Stepavoidance(Vector3 bulletpos ,bool);//弾丸をステップで回避したい(boolで左右を取る)
	void MoveStep(Vector3 Movevec);//ベクトルを入れてステップ踏ませたい

	Vector3 GetForward() { return Forward_vec; }
	Vector3 GetRight() { return Right_vec; }
	Vector3 GetUp() { return Up_vec; }

	int GetHP() { return HP; }
	int GetMaxHP() { return MaxHP; }
	void SetPlayer(M_Player* pl);
	GM31::GE::Collision::BoundingBoxOBB GetOBB_Bullet(int bulletnum);
	bool GetBulletcol(int bulletnum) { return e_missiles[bulletnum].GetCollsion(); }
	GM31::GE::Collision::BoundingBoxOBB GetOBB_Beam();

	Vector3 GetCenter() { return m_Position - Forward_vec * 1.5f; }//当たり判定の中心

	void SetCollision(bool col);
	void SetCollision_Bullet(int,bool);
	void SetAvoidance(bool inter) { Avoidance = inter; }

	int Damage_Bullet() { return ATK_Bullet; }
	int Damage_Beam() { return ATK_Beam; }
};