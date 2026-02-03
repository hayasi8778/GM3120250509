#pragma once
#include"Enemy_Missile.h"
#include"Player_Mec.h"
#include"Gun.h"

#include "system/DebugUI.h"


//プレイヤーの挙動やステータスを取得してエネミーに挙動を返す
struct EnemyThinking
{
private:
	Vector3* PlayerPosition;//プレイヤーの座標
	M_Player* Player;//プレイヤー()
	Vector3 CurentPos_P;//プレイヤーの1フレーム前の座標

	//テスト用変数
	Vector3 Testvec = Vector3::Zero;

	Enemy_Missile Enemy;//敵
	M_Gun* GunObject[8] = { nullptr ,nullptr ,nullptr ,nullptr ,nullptr,nullptr ,nullptr ,nullptr };
	int TotalGun = 0;
	int EnemyState;//敵の状態を取得しておいておく
	int PlayerState;//プレイヤーの状態
	//プレイヤーの動きの苛烈さに対して敵の動きのレベル上げたいのでプレイヤーの動きで加算して敵の動きの上限作る
	int Strength = 0;
	int ShotStrength = 1;
	int MoveStrength = 1;
	int ShotLevel = 0;//敵の強さを行動で増減する数値と安易に増減しないレベルで管理する

	int MoveLevel = 0;//敵の強さを行動で増減する数値と安易に増減しないレベルで管理する
	//レベルの変動するかのフラグ
	bool LevelLock_Shot = false;
	bool LevelLock_Move = false;
	//行動で増える量
	int ShotIncrease = 200;
	int MoveIncrease = 1;
	int AvoidanceCost = 300;


	Vector3 PositionLog[300]; //5秒間(60fpsで計測)のプレイヤーと敵の距離を記録
	Vector3 FirstRange = { 0.0f,0.0f,0.0f };//初手の距離を記録しておく
	int LogSubscript = 0; //座標ログの添え字
	float TimeLog = 0;//時間経過を取得
	float cooltime = 0;//1/60秒ごとに判定を挟むための変数
	float shotcool = 500;//射撃連打すると難易度爆上がりするのを抑制するための変数
	bool Think = true;//思考ベースのAIを使うか

	//int Level = 0;//AIを切り替える用に変数用意しておく
public:
	void DebugUI();//GUI
	void Init(M_Player* pl);
	void Update(uint64_t);
	void LateUpdate(uint64_t);
	void Draw();
	void Reset();
	void Action(Vector3);

	
	void AddGun(M_Gun*);
	Enemy_Missile* GetEnemy() { return &Enemy; }
	void RuleUpdate(uint64_t);//ルールベースのAI
	void ThinkUpdate(uint64_t);//プレイヤーの動きに対して強化されるAI
	//移動
	void ThinkMove(uint64_t);
	//回避
	void ThinkEvasion(uint64_t);
	void ThinkShot(uint64_t);
	void LevelControl();
	void Collision();

	bool GetSpecial() { return Enemy.GetSpecialFlag(); }
};



