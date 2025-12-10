#pragma once
#include "Object.h"
#include "Buller.h"
#include <random>

//封印されしロボゾディア
#include "Player_Head.h"
#include "Player_LeftArm.h"
#include "Player_RightArm.h"
#include "Player_LeftFeet.h"
#include "Player_RightFeet.h"

#include "system/DebugUI.h"


class M_Player : public Object 
{
private:
	Object* Connectableobject = nullptr;//接続されたオブジェクト

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	//最終的にはここにまとめる形にしたい
	const int MaxBullets = 20;
	Bullet m_bullets[20];
	bool Burst = false;
	float BurstCoolTime = 0;
	float BurstCount = 0;
	int bulletcur = 0;

	int MaxHP = 100;
	int HP = MaxHP;//体力
	float Invincibility_time = 0;//無敵時間

	Vector3* Target = nullptr;
	
	float time = 0;//経過時間を加算して計測する
	bool Shot = false; //射撃したフレームにonになる関数

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width;
	float Height;
	float Depth;

	bool col = false; //当たり判定取るためのやつ

	Player_Head head;
	Player_LeftArm leftarm;
	Player_RightArm rightarm;
	Player_LeftFeet leftfeet;
	Player_RightFeet rightfeet;

	//オブジェクトがセットされていることを判定するフラグ
	bool HeadSet = false;
	bool BodySet = false;
	bool LeftArmSet = false;
	bool RightArmSet = false;
	bool LeftFeetSet = false;
	bool RightFeetSet = false;

	int DoublePistol = 3;//二丁拳銃を交互に打つための変数

public:
	int TestInt = 0;//デバックであると便利だから作っておく

	void Init() override;
	void Update(uint64_t deltatime) override;
	void LateUpdate(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;
	void Reset();
	int GetShaderNum() override;
	Vector3 GetForward();
	Vector3 GetRight();
	Vector3 GetUp();

	void Debug_Player();//デバック用GUI一式

	void SpecialAttack(uint64_t);

	int GetHP() { return HP; }
	int GetMaxHP() { return MaxHP; }
	void SetTarget(Vector3* tar);
	Vector3* GetTarget() { return Target; }
	void SetCol(bool collision) { if (collision) { col = collision; } };
	void HitDamage(int damage) { if (col) HP -= damage; }
	void SetBurst(bool br) { Burst = br; }

	Vector3 ConectPos();
	Vector3 ConectPos(int i);
	//当たり判定のために描画時の角度情報返す関数を作っておく
	Vector3 GetRotation_col() { Vector3 rotcop = m_Rotation; 
	rotcop.x += 1.55f; rotcop.y += 1.55f; 
	return rotcop;}
	bool GetShot() { return Shot; }

	//無敵時間中かどうかの判定
	bool GetInvincibility() { if (Invincibility_time != 0) { return true; } return false; }

	void SetRotation_PL(Vector3);

	bool Connectable(int);
	
	bool Collision_PL(GM31::GE::Collision::BoundingBoxOBB colobb);

	bool DrawBone = false;
	bool DrawModel = true;

	int GetDoublePistol() { return DoublePistol; }
	//取り付け可否の判断
	int Canconect(Object*);
	//オブジェクトの取り付け
	void Conect(int, Object*);
	void Release(int);
	void ReleaseALL();

	//void ModelAABB(aiVector3D& outMin, aiVector3D& outMax);
		
};