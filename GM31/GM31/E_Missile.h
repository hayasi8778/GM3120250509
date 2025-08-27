#pragma once
#include "Object.h"
#include "Player_Mec.h"

class E_Missile : public Object
{
private:

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	float boom_time = 0;//爆発の残る時間

	bool shot = false;//発射フラグ

	Vector3 forward;//前方ベクトルを取得するための変数

	Vector3 velocty = Vector3{0,0,0};

	M_Player* player = nullptr;

	bool collsion = false;

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	std::unique_ptr<Sphere> Boooooooom;// 弾が当たった後に爆発させたい
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

	void SetShot(bool st) { shot = st; collsion = false;}//フラグのリセット
	void SetForward(Vector3 forw) { forward = forw; }
	Vector3 SetForward() { return forward; }
	int Life = 120;
	bool erase = false;
	float priod = 1000;
	void SetPlayar(M_Player* pl) { player = pl; }
	void SetCol(bool col) { collsion = col; }
};