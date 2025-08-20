#pragma once
#include "Object.h"
#include "Buller.h"
#include <random>

#include "system/DebugUI.h"


class M_Player : public Object 
{
private:
	Bullet m_bullet[20];

	Vector3* Target = nullptr;
	bool Burst = false;
	int bulletcur = 0;
	float time = 0;//経過時間を加算して計測する
	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width;
	float Height;
	float Depth;

	bool col = false; //当たり判定取るためのやつ
public:
	void Init() override;
	void Update(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;
	Vector3 GetForward();

	void SetTarget(Vector3* tar) { Target = tar; }
	Vector3* GetTarget() { return Target; }
	void SetCol(bool collision) { col = collision; }
	void Debug_Player();//デバック用GUI一式
	Vector3 ConectPos();
	void FullBurst();
	//当たり判定のために描画時の角度情報返す関数を作っておく
	Vector3 GetRotation_col() { Vector3 rotcop = m_Rotation; 
	rotcop.x += 1.55; rotcop.y += 1.55; 
	return rotcop;}

	bool DrawBone = false;
	bool DrawModel = true;

	//void ModelAABB(aiVector3D& outMin, aiVector3D& outMax);
		
};