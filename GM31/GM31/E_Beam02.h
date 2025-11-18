#pragma once
#pragma once
#include "Object.h"
#include "Player_Mec.h"

class E_Beam02 : public Object
{
private:

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	float boom_time = 0;//爆発の残る時間

	bool collsion = false;

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	std::unique_ptr<Sphere> Boooooooom;// 弾が当たった後に爆発させたい
	float Width;
	float Height;
	float Depth;
public:
	E_Beam02();
	~E_Beam02();
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

	void SetRightVec(Vector3 rightvec) { Right_vec = rightvec; }
	void SetUpVec(Vector3 upvec) { Up_vec = upvec; }
	void SetForwardVec(Vector3 forwardvec) { Forward_vec = forwardvec; }
};