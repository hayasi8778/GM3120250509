#pragma once
#include "Object.h"

class Enemy_Head :public Object
{
private:

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width = 0;
	float Height = 0;
	float Depth = 0;

	bool col = false; //当たり判定取るためのやつ
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

	void SetMoveState(int state);

	Vector3 Conectpos(const std::string& targetName);
};