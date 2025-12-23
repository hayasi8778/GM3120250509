#include "EnemyAI.h"
#include <iostream>

//float GetRange(Vector3 vecA, Vector3 vecB) {
//	Vector3 coppos_A = vecA;
//	Vector3 coppos_B = vecB;
//	if (coppos_A.x < 0) coppos_A.x *= -1;
//	if (coppos_A.y < 0) coppos_A.y *= -1;
//	if (coppos_A.z < 0) coppos_A.z *= -1;
//
//	if (coppos_B.x < 0) coppos_B.x *= -1;
//	if (coppos_B.y < 0) coppos_B.y *= -1;
//	if (coppos_B.z < 0) coppos_B.z *= -1;
//	Vector3 ranged = { coppos_A.x - coppos_B.x , coppos_A.y - coppos_B.y , coppos_A.z - coppos_B.z };
//	if (ranged.x < 0) ranged.x *= -1;
//	if (ranged.y < 0) ranged.y *= -1;
//	if (ranged.z < 0) ranged.z *= -1;
//	float rangedALL = ranged.x + ranged.y + ranged.z;
//
//	return rangedALL;
//}

void EnemyThinking::DebugUI()
{
	ImGui::Begin("ThinkChanger");

	ImGui::Text("Enemy");
	ImGui::Checkbox(std::string("Think").c_str(), &Think);
	ImGui::Text("Strength: %d", Strength);
	if (ImGui::Button("AddStrongth+1000")) 
	{
		ShotStrength += 500;
		MoveStrength += 500;
	}
	ImGui::SliderInt("ShotIncrease", &ShotIncrease, 0, 500);
	
	ImGui::Text("ShotLevel: % d", ShotLevel);
	if (ImGui::Button("ShotLebelUp")) {
		if (ShotLevel < 7)ShotLevel++;
		Enemy.SetShotState(ShotLevel);
	}
	if (ImGui::Button("ShotLebelDown")) {
		if (ShotLevel > 0)ShotLevel--;
		Enemy.SetShotState(ShotLevel);
	}

	ImGui::Text("MoveLevel: % d", MoveLevel);

	ImGui::Text("ShotStrength: %d", ShotStrength);
	ImGui::Text("MoveStrength: %d", MoveStrength);

	ImGui::End();
}

void EnemyThinking::Init(M_Player* pl)
{
	Player = pl;
	Enemy.Init();
	Enemy.SetPlayer(Player);

	Vector3 playerpos = Player->GetPosition();
	playerpos.z -= 20;

	Player->SetTarget(Enemy.GetPosition_P());
	Vector3 right = Player->GetRight();
	Vector3 up = Player->GetUp();
	Vector3 forward = Player->GetForward();

	CurentPos_P = Player->GetPosition();

	//敵の方向いてないっぽいから一旦敵の方向く可能性あるかも
	//Vector3 pos = Player->GetPosition() - Enemy.GetPosition();
	Vector3 pos = Enemy.GetPosition() - playerpos;
	Vector3 localpos;
	localpos.x = Dot(pos, Player->GetRight());
	localpos.y = Dot(pos, Player->GetUp());
	localpos.z = Dot(pos, Player->GetForward());
	FirstRange = localpos;
	//初期座標を入れておく
	for (int i = 0; i < 300; i++) {
		PositionLog[i] = FirstRange;
	}
	
	#ifdef _DEBUG
	//GUI関連
	DebugUI::RedistDebugFunction([this]() {
		DebugUI();
		});
	#endif
	

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
	//距離関係を初期値にリセットしておく
	for (int i = 0; i < 300; i++) {
		PositionLog[i] = FirstRange;
	}

	Enemy.Reset();
	Enemy.SetPosition({ 0,10,50 });
	Strength = 0;
	ShotStrength = 0;
	MoveStrength = 0;
	ShotLevel = 0;
	MoveLevel = 0;
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
				Enemy.SetCollision(col, gun->Damage_Bullet());
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
	//経過時間を記録
	float time = static_cast<float>(deltatime) / 1000;
	//射撃を取得してきて加算(連打でレベルが上がるのでクールタイムを加味する)
	if (shotcool == 500) { if (Player->GetShot()) { 
		//Strength += ShotIncrease;  
		ShotStrength += ShotIncrease;
		shotcool -= time; } }
	else { shotcool -= time; 
	if (shotcool < 0)shotcool = 500;
	}
	
	Enemy.Update(deltatime);
	ThinkMove(deltatime);
	ThinkShot(deltatime);
	Enemy.Timer(deltatime);
	LevelControl();

	//プレイヤーが動いていた場合
	if (GetRange(CurentPos_P, Player->GetPosition()) > 1) {
		//Strength += MoveIncrease;//強さの数値を加算
		MoveStrength += MoveIncrease;
	}

	//プレイヤーの過去座標として記録
	CurentPos_P = Player->GetPosition();

	Collision();

	//最後にStrengthの処理を通す
	Strength = ShotStrength + MoveStrength;
}

void EnemyThinking::ThinkMove(uint64_t deltatime)
{
	Vector3 pos_PL = Player->GetPosition();
	Vector3 pos_EN = Enemy.GetPosition();

	TimeLog += static_cast<float>(deltatime) / 1000;

	//ログの更新
	if (TimeLog > cooltime + (1000 / 60.0f)) {
		Vector3 localpos;
		localpos.x = Dot(pos_EN - pos_PL, Player->GetRight());
		localpos.y = Dot(pos_EN - pos_PL, Player->GetUp());
		localpos.z = Dot(pos_EN - pos_PL, Player->GetForward());
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
	Vector3 Targetpos = pos - Player->GetPosition();//positionLogに保存された場所に移動する

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
			
			if (ShotStrength > 300 /*&& range >30 */ )
			{

				ShotStrength -= AvoidanceCost;
				Enemy.SetAvoidance(true);
				Enemy.Stepavoidance(minposition, false);
			}
			else Enemy.Move(Enemy.GetPosition() - Targetright * 100);
		}
		else 
		{
			//射撃部分の判定のみを取って計算
			if (ShotStrength > 300 /*&& range > 30 */ )
			{

				ShotStrength -= AvoidanceCost;
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

void EnemyThinking::ThinkShot(uint64_t dt)
{
	//射撃は大技→通常射撃の順で行う
	if (Strength > 1000 && Enemy.GetFIRE() && !Enemy.GetSpecialFlag()) {
		//Strength -= 1000;
		//ShotとMoveの割合で合計1000引く
		float test = float(ShotStrength) / float(Strength);
		test = float(MoveStrength / Strength);

		ShotStrength -= float(ShotStrength) / float(Strength) * 1000;
		MoveStrength -= float(MoveStrength) / float(Strength) * 1000;

		Enemy.SetSpecialFlag(true);

		//Enemy.Shot(dt);

	}

	if (Strength > 40 && Enemy.GetFIRE() && !Enemy.GetSpecialFlag()) {
		//Strength -= 40;
		if (MoveStrength > 40) MoveStrength -= 40;
		else {
			ShotStrength -= float(ShotStrength) / float(Strength) * 40;
			MoveStrength -= float(MoveStrength) / float(Strength) * 40;
		}
		if(ShotLevel <2)Enemy.Shot(dt);
		else {
			Vector3 player_vec = Player->GetPosition() - CurentPos_P;
			player_vec.Normalize();
			Vector3 Targetvec = Enemy.GetPosition() - (Player->GetPosition() + player_vec* (Player->GetSpead() + 6.0f));
			if(GetRange(CurentPos_P, Player->GetPosition()) > 1){
				Enemy.Shot(dt, Targetvec);
			}
			Enemy.Shot(dt);
		}
	}

	
}

void EnemyThinking::LevelControl()
{
	//レベルの調整(レベルは下がらないものとして扱う)
	//射撃部分
	if ((MoveStrength > 600 || Enemy.GetHP() != Enemy.GetMaxHP()) && ShotLevel < 2) { ShotLevel = 2; Enemy.SetShotState(ShotLevel); }
	if (ShotLevel == 2 && MoveStrength > 500 && ShotStrength > 400) { ShotLevel = 3; Enemy.SetShotState(ShotLevel);}

	//移動部分のレベル調整
	if (ShotStrength > 600 && MoveLevel < 2) { MoveLevel = 2;  AvoidanceCost = 100; Enemy.SetMoveState(MoveLevel); }
	//HPを削った場合はその時点でレベル2に行く
	if(Enemy.GetHP() != Enemy.GetMaxHP()) { MoveLevel = 2;  AvoidanceCost = 100; Enemy.SetMoveState(MoveLevel); }
	if (ShotStrength > 800 && MoveLevel < 2) { MoveLevel = 3;  AvoidanceCost = 50; Enemy.SetMoveState(MoveLevel); }

	//プレイヤーが有利過ぎたらレベルに上方修正掛ける
	//if(Player->GetHP() - Enemy.GetHP())
}

void EnemyThinking::Collision()
{
	//当たり判定処理
	for (int i = 0; i < TotalGun; i++) //銃を親オブジェクトとした弾と敵の当たり判定
	{
		if (!GunObject[i]) continue;//ネスト長くなるからここはifの中身じゃなくてcontinueで返す
		if (auto gun = GunObject[i])
		{
			for (int i = 0; i < 5; i++)
			{
				bool col = GM31::GE::Collision::CollisionOBB(Enemy.GetOBB(), gun->GetOBB_Bullet(i));
				Enemy.SetCollision(col, gun->Damage_Bullet());
				if (col) gun->SetCollision_Bullet(i, col);
			}
		}
	}

	for (int i = 0; i < 20; i++) {
		bool col = GM31::GE::Collision::CollisionOBB(Enemy.GetOBB(), Player->GetOBB_Bullet(i));
		Enemy.SetCollision(col, 25);
		if (col) Player->SetCollision_Bullet(i, col);
	}
}
