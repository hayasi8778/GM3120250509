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

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	Bullet m_bullet[20];

	int HP = 5;//体力
	float Invincibility_time = 0;//無敵時間

	Vector3* Target = nullptr;
	bool Burst = false;
	int bulletcur = 0;
	float time = 0;//経過時間を加算して計測する

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
public:
	int TestInt = 0;//デバックであると便利だから作っておく

	void Init() override;
	void Update(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB_Bullet(int i);
	Vector3 GetForward();

	void SetTarget(Vector3* tar) { Target = tar; }
	Vector3* GetTarget() { return Target; }
	void SetCol(bool collision) { if(collision)col = collision; }
	void Debug_Player();//デバック用GUI一式
	Vector3 ConectPos();
	void FullBurst();
	//当たり判定のために描画時の角度情報返す関数を作っておく
	Vector3 GetRotation_col() { Vector3 rotcop = m_Rotation; 
	rotcop.x += 1.55; rotcop.y += 1.55; 
	return rotcop;}

	void SetRotation_PL(Vector3);
	
	bool Collision_PL(GM31::GE::Collision::BoundingBoxOBB colobb);

	void SetCollision_Bullet(int, bool);

	bool DrawBone = false;
	bool DrawModel = true;

	//void ModelAABB(aiVector3D& outMin, aiVector3D& outMax);
		
};