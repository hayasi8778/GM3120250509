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
	Enemy_Missile Enemy;//敵
	M_Gun* GunObject[8] = { nullptr ,nullptr ,nullptr ,nullptr ,nullptr,nullptr ,nullptr ,nullptr };
	int TotalGun = 0;
	int EnemyState;//敵の状態を取得しておいておく
	int PlayerState;//プレイヤーの状態
	//プレイヤーの動きの苛烈さに対して敵の動きのレベル上げたいのでプレイヤーの動きで加算して敵の動きの上限作る
	int Boost = 0;

	Vector3 PositionLog[300]; //5秒間(60fpsで計測)のプレイヤーと敵の距離を記録
	int LogSubscript = 0; //座標ログの添え字
	float TimeLog = 0;//時間経過を取得
	float cooltime = 0;//1/60秒ごとに判定を挟むための変数
	bool Think = true;//思考ベースのAIを使うか

	int Level = 0;//AIを切り替える用に変数用意しておく
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

};



