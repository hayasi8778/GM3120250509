#pragma once
#include "Object.h"

class Enemy_RightArm :public Object
{
private:

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	float Armrot = +1.40f;
	bool armfloat = true;

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width;
	float Height;
	float Depth;

	bool col = false; //当たり判定取るためのやつ
	float recoil = 0.0f;//銃を撃った時の反動
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

	Vector3 Conectpos(const std::string& targetName);
	void Rockon(Vector3 rot);
	
};