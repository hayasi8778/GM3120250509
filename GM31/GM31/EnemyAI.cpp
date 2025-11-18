#include "EnemyAI.h"
#include <iostream>

// inline/constexpr にしてコンパイラ最適化を効かせる
inline float Dot(const Vector3& a, const Vector3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

void EnemyThinking::DebugUI()
{
	ImGui::Begin("ThinkChanger");

	ImGui::Text("Enemy");
	ImGui::Checkbox(std::string("Think").c_str(), &Think);

	// カメラの位置を極座標からデカルト座標に変換
	ImGui::End();
}

void EnemyThinking::Init(M_Player* pl)
{
	Player = pl;
	Enemy.Init();
	Enemy.SetPlayer(Player);



	Player->SetTarget(Enemy.GetPosition_P());
	Vector3 right = Player->GetRight();
	Vector3 up = Player->GetUp();
	Vector3 forward = Player->GetForward();

	//敵の方向いてないっぽいから一旦敵の方向く可能性あるかも
	Vector3 pos = Player->GetPosition() - Enemy.GetPosition();
	Vector3 localpos;
	localpos.x = Dot(pos, Player->GetRight());
	localpos.y = Dot(pos, Player->GetUp());
	localpos.z = Dot(pos, Player->GetForward());
	//初期座標を入れておく
	for (int i = 0; i < 300; i++) {
		PositionLog[i] = localpos;
	}

	//GUI関連
	DebugUI::RedistDebugFunction([this]() {
		DebugUI();
		});

	

}

void EnemyThinking::Update(uint64_t deltatime) 
{
	if (Think) ThinkUpdate(deltatime);//思考による敵制御
	else RuleUpdate(deltatime);//ルールベースの敵
}

void EnemyThinking::LateUpdate(uint64_t deltatime) 
{
	Enemy.LateUpdate(deltatime);
}

void EnemyThinking::Draw() 
{
	Enemy.Draw();
}

void EnemyThinking::Reset() 
{
	Enemy.Reset();
	Enemy.SetPosition({ 0,10,50 });
}

void EnemyThinking::Action(Vector3 vec)
{
	Enemy.Action(vec);
}



void EnemyThinking::AddGun(M_Gun* gun)
{
	if (TotalGun >= 8) return;//GunObjectのサイズを超過するなら処理しない
	GunObject[TotalGun] = gun;
	TotalGun++;
}

void EnemyThinking::RuleUpdate(uint64_t deltatime)
{
	Enemy.Update(deltatime);
	Enemy.Move();
	Enemy.Timer(deltatime);
	Enemy.Shot_Rule(deltatime);

	//当たり判定処理
	for (int i = 0; i < TotalGun; i++) //銃を親オブジェクトとした弾と敵の当たり判定
	{
		if (!GunObject[i]) continue;//ネスト長くなるからここはifの中身じゃなくてcontinueで返す
		if (auto gun = GunObject[i])
		{
			for (int i = 0; i < 5; i++)
			{
				bool col = GM31::GE::Collision::CollisionOBB(Enemy.GetOBB(), gun->GetOBB_Bullet(i));
				//索敵範囲内に弾丸があるかどうか
				bool inter = GM31::GE::Collision::CollisionSphereOBB_(Enemy.GetShere(), gun->GetOBB_Bullet(i));
				Enemy.SetCollision(col);
				if (col) gun->SetCollision_Bullet(i, col);
				if (inter) {
					Enemy.SetAvoidance(inter);
					Enemy.Stepavoidance(gun->GetPosition(),true);
					//gun->SetCollision_Bullet(i, inter);
				}
			}
		}
	}
}

void EnemyThinking::ThinkUpdate(uint64_t deltatime)
{
	if (Player->GetShot())Boost += 20;
	
	Enemy.Update(deltatime);
	ThinkMove(deltatime);
	Enemy.Timer(deltatime);


	//当たり判定処理
	for (int i = 0; i < TotalGun; i++) //銃を親オブジェクトとした弾と敵の当たり判定
	{
		if (!GunObject[i]) continue;//ネスト長くなるからここはifの中身じゃなくてcontinueで返す
		if (auto gun = GunObject[i])
		{
			for (int i = 0; i < 5; i++)
			{
				bool col = GM31::GE::Collision::CollisionOBB(Enemy.GetOBB(), gun->GetOBB_Bullet(i));
				Enemy.SetCollision(col);
				if (col) gun->SetCollision_Bullet(i, col);
			}
		}
	}
}

void EnemyThinking::ThinkMove(uint64_t deltatime)
{
	Vector3 pos_PL = Player->GetPosition();
	Vector3 pos_EN = Enemy.GetPosition();

	TimeLog += static_cast<float>(deltatime) / 1000;

	if (TimeLog > cooltime + (1000 / 60.0f)) {
		Vector3 localpos;
		localpos.x = Dot(pos_PL - pos_EN, Player->GetRight());
		localpos.y = Dot(pos_PL - pos_EN, Player->GetUp());
		localpos.z = Dot(pos_PL - pos_EN, Player->GetForward());
		PositionLog[LogSubscript] = localpos;

		cooltime = TimeLog;
		LogSubscript++;
	}

	//5秒経過
	if (TimeLog > 5000)
	{
		LogSubscript = 0;//添え字の初期化
		cooltime = 0;
		TimeLog = 0;

		//std::cout << "座標計測リセット" << std::endl;
	}
	//プレイヤー座標-敵座標を保存する
	Vector3 pos = Vector3(0, 0, 0);
	for (int i = 0; i < 300; i++) //
	{
		pos += PositionLog[i];
	}
	//距離の平均値出す
	pos /= 300;

	//プレイヤーのローカル座標系の座標をワールド座標系へ復元
	pos = Player->GetPosition() + Player->GetRight() * pos.x + Player->GetUp() * pos.y + Player->GetForward() * pos.z;
	Vector3 Targetpos = pos - Player->GetPosition();//

	//当たり判定処理
	Vector3 minposition = { 0,0,0 };
	float range = 1000.0f;
	for (int i = 0; i < TotalGun; i++) //銃を親オブジェクトとした弾と敵の当たり判定
	{
		if (!GunObject[i]) continue;//ネスト長くなるからここはifの中身じゃなくてcontinueで返す
		if (auto gun = GunObject[i])
		{
			for (int i = 0; i < 5; i++)
			{
				//弾丸を見てかわす
				bool inter = GM31::GE::Collision::CollisionSphereOBB_(Enemy.GetShere(), gun->GetOBB_Bullet(i));
				if (inter) {
					//かわしやすい方に移動してかわす
					Vector3 coppos_B = gun->GetBulletpos(i);
					Vector3 coppos_E = Enemy.GetPosition();
					if (coppos_B.x < 0) coppos_B.x *= -1;
					if (coppos_B.y < 0) coppos_B.y *= -1;
					if (coppos_B.z < 0) coppos_B.z *= -1;

					if (coppos_E.x < 0) coppos_E.x *= -1;
					if (coppos_E.y < 0) coppos_E.y *= -1;
					if (coppos_E.z < 0) coppos_E.z *= -1;
					//Y軸は直接移動できないからXZの2軸判定
					Vector3 ranged = { coppos_B.x - coppos_E.x , coppos_B.y - coppos_E.y , coppos_B.z - coppos_E.z };
					if (ranged.x < 0) ranged.x *= -1;
					if (ranged.y < 0) ranged.y *= -1;
					if (ranged.z < 0) ranged.z *= -1;
					float rangedALL = ranged.x + ranged.y + ranged.z;

					if (range > rangedALL) {
						minposition = gun->GetBulletpos(i);
						range = rangedALL;
					}
					
					//gun->SetCollision_Bullet(i, inter);
				}
			}
		}
	}

	//範囲内に弾丸があった場合実行
	if (range != 1000.0f) {
		Vector3 localpos = minposition;
		localpos.x = Dot(localpos, Enemy.GetRight());
		localpos.y = Dot(localpos, Enemy.GetUp());
		localpos.z = Dot(localpos, Enemy.GetForward());

		//最小座標に対しての右向きベクトルの取得
		Vector3 MoveVec = minposition - Enemy.GetPosition();
		MoveVec.y = 0;//正規化前にy軸を切る
		MoveVec.Normalize();

		Vector3 up(0.0f, 1.0f, 0.0f); // ワールドの上方向
		Vector3 Targetright = MoveVec.Cross(up);
		Targetright.Normalize();      // 正規化して右向きベクトル完成

		if (localpos.x > 0) //ローカル座標に合わせて左右の判定をする
		{
			
			if (Boost > 30 && range >30)
			{

				Boost -= 30;
				Enemy.SetAvoidance(true);
				Enemy.Stepavoidance(minposition, false);
			}
			else Enemy.Move(Enemy.GetPosition() - Targetright * 100);
		}
		else 
		{
			if (Boost > 30 && range > 30)
			{

				Boost -= 30;
				Enemy.SetAvoidance(true);
				Enemy.Stepavoidance(minposition, true);
			}
			else Enemy.Move(Enemy.GetPosition() + Targetright * 100);
			
		}
		//Enemy.SetAvoidance(true);
		//Enemy.Stepavoidance(gun->GetPosition());
	}else Enemy.Move(Targetpos);

	////一定以上攻撃的ならステップ踏む
	//if (Boost > 30)
	//{
	//	
	//	Boost -= 30;
	//	Enemy.MoveStep(-Enemy.GetRight());
	//}

	
}
