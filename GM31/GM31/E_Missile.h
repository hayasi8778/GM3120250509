#pragma once
#include "Object.h"
#include "Player_Mec.h"

class E_Missile : public Object
{
private:
	bool shot = false;//発射フラグ

	Vector3 forward;//前方ベクトルを取得するための変数

	Vector3 velocty = Vector3{0,0,0};

	M_Player* player = nullptr;

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	float Width;
	float Height;
	float Depth;
public:
	E_Missile();
	~E_Missile();
	void Init() override;
	void Update(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;

	void SetShot(bool st) { shot = st; }
	void SetForward(Vector3 forw) { forward = forw; }
	Vector3 SetForward() { return forward; }
	int Life = 120;
	bool erase = false;
	float priod = 1000;
	void SetPlayar(M_Player* pl) { player = pl; }
};