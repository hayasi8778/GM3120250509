#pragma once
#include "Object.h"
#include "Player_Mec.h"

#include "blobshadow.h"

#include "system/CSprite.h"


class E_Missile : public Object
{
private:

	//方向ベクトル
	Vector3 Right_vec;
	Vector3 Up_vec;
	Vector3 Forward_vec;

	float boom_time = 0;//爆発の残る時間
	float Alive_time = 5000;//生存時間

	bool shot = false;//発射フラグ

	Vector3 forward;//前方ベクトルを取得するための変数

	Vector3 velocty = Vector3{0,0,0};

	Object* player = nullptr;

	bool collsion = false;

	int count = 3;//カウントダウンに使う奴
	const float DefaultShotSpeed = 0.1f;//デフォの弾速
	float ShotSpeed = 0.1f;
	
	const float DefaultMaxTurn = 0.01f; // 1フレームで回せる最大角度（ラジアン）
	float maxTurn = 0.01f; // 1フレームで回せる最大角度（ラジアン)
	float Turn = 0.01f; // 2段階に追尾するための関数
	float Turn_Time = 0;	//いつまで追尾性上げたままにするか

	std::unique_ptr<Box> m_shapecube_col;// 当たり判定(ボックス)
	std::unique_ptr<Sphere> Boooooooom;// 弾が当たった後に爆発させたい
	float Width = 0;
	float Height = 0;
	float Depth = 0;

	//雑に板ポリで影を出す
	std::unique_ptr<CSprite> m_Shadow;
	//std::unique_ptr<BlobShadow> m_BlobShadow;
public:
	E_Missile();
	~E_Missile();
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
	void ResetVector();//3軸ベクトルを取得しなおす

	void SetShot(bool st) { shot = st; collsion = false;}//フラグのリセット
	void SetForward(Vector3 forw) { forward = forw; }
	Vector3 SetForward() { return forward; }
	int Life = 120;
	bool erase = false;
	float priod = 1000;
	void SetObject(Object* pl);
	void SetCol(bool col) { collsion = col; }
	bool GetCollsion() { return collsion; }
	//最初の何フレーム分補正を挟むか
	void SetCount(int cnt) { count = cnt; }

	//最初の誘導をどれだけの時間どれくらい誘導するかを決める関数
	void SetmaxTurn(float turn, float time) { maxTurn = turn; Turn_Time = time; }
	//一定時間経った後の誘導値を決める
	void SetTurn(float turn) { Turn = turn;}
	void SetShotSpeed(float speed) { ShotSpeed = speed; }
	void Update_Shot1(uint64_t deltatime);
	void Update_Shot2(uint64_t deltatime);
};