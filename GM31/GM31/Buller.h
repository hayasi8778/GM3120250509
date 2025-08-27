#pragma once
#include "Object.h"

class Bullet : public Object
{
private:

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	Vector3 forward;//前方ベクトルを取得するための変数
	Vector3 velocty = Vector3{ 0,0,0 };
	Vector3 Targetpos = Vector3{ 0,0,0 };
	Vector3* Target_P = nullptr;

	bool induction = false;
	float priod = 0;
	float filstpriod = 0;

	float boom_time = 0;//爆発の残る時間

	bool shot = false;//発射フラグ
	bool Missile = true;//誘導開始の1フレ目に処理入れたいからフラグ作る

	bool collsion = false;//当たり判定

	std::unique_ptr<Box> m_shapecube_col;// 弾の当たり判定(ボックス)
	std::unique_ptr<Sphere> Boooooooom;// 弾が当たった後に爆発させたい
	float Width;
	float Height;
	float Depth;
public:
	Bullet();
	~Bullet();
	void Init() override;

	void Update(uint64_t deltatime) override;
	void Draw() override;
	void Dispose() override;
	void Adhesioing() override;
	void Action(Vector3 vec) override;
	GM31::GE::Collision::BoundingBoxOBB GetOBB() override;

	void SetForward(Vector3 forw) { forward = forw; shot = true; collsion = false; }
	Vector3 GetForward() { return forward; }
	void SetTarget(Vector3 pos) { Targetpos = pos; }
	void SetTarget(Vector3* pos) { Target_P = pos; }
	void Setinduction(float ,Vector3);
	void SetCol(bool col) { collsion = col; }
	int Life = 120;
	bool erase = false;
};