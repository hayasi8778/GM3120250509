#pragma once
#include "Object.h"
#include "Player_Mec.h"

class M_Rock : public Object 
{
private:
	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	bool shootflag = false;
	bool returnRock = false;//射出から帰ってくるための帰還フラグ
	const int DefaultLife = 500;
	int life = 0;
	float priod;
	Vector3 forward;//前向きに射出するためのベクトル
	Vector3 velocty;//加速値
	M_Player* player = nullptr;
	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width;
	float Height;
	float Depth;
public:
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
	void SetPlayer(M_Player* pl);
};