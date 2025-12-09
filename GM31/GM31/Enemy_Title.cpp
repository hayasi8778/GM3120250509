#include "Enemy_Title.h"
#include <iostream>

void EnemyThinking_Title::DebugUI()
{
	ImGui::Begin("TitleEnemys");

	ImGui::Text("Enemy1");
	ImGui::Checkbox(std::string("Think").c_str(), &Think);
	ImGui::Text("Strength_Enemy1: %d", Strength_Enemy1);
	if (ImGui::Button("AddStrongth_1+1000"))
	{
		Strength_Enemy1 += 1000;
	}

	ImGui::Text("Enemy2");
	ImGui::Text("Strength_Enemy2: %d", Strength_Enemy2);
	if (ImGui::Button("AddStrongth_2+1000"))
	{
		Strength_Enemy2 += 1000;
	}

	ImGui::End();
}

void EnemyThinking_Title::Init()
{
	Enemy1.Init();
	Enemy1.SetPatner(&Enemy2);

	Enemy2.Init();
	Enemy2.SetPatner(&Enemy1);
	Enemy2.SetPosition({ 20.0f,10.0f,0.0f });

	Enemy1.SetShotState(2);
	Enemy2.SetShotState(3);

	Vector3 right = Enemy2.GetRight();
	Vector3 up = Enemy2.GetUp();
	Vector3 forward = Enemy2.GetForward();

	CurentPos_Enemy2 = Enemy2.GetPosition();

	//敵の方向いてないっぽいから一旦敵の方向く可能性あるかも
	Vector3 pos = Enemy2.GetPosition() - Enemy1.GetPosition();
	Vector3 localpos;
	localpos.x = Dot(pos, Enemy2.GetRight());
	localpos.y = Dot(pos, Enemy2.GetUp());
	localpos.z = Dot(pos, Enemy2.GetForward());
	//初期座標を入れておく
	for (int i = 0; i < 300; i++) {
		PositionLog[i] = localpos;
	}

	//GUI関連
	DebugUI::RedistDebugFunction([this]() {
		DebugUI();
		});



}

void EnemyThinking_Title::Update(uint64_t deltatime)
{
	if (Think) ThinkUpdate(deltatime);//思考による敵制御
	else RuleUpdate(deltatime);//ルールベースの敵
}

void EnemyThinking_Title::LateUpdate(uint64_t deltatime)
{
	Enemy1.LateUpdate(deltatime);
	Enemy2.LateUpdate(deltatime);
}

void EnemyThinking_Title::Draw()
{
	Enemy1.Draw();
	Enemy2.Draw();
}

void EnemyThinking_Title::Reset()
{
	Enemy1.Reset();
	Enemy1.SetPosition({ 0,10,50 });
	Strength_Enemy1 = 0;

	Enemy2.Reset();
	Enemy2.SetPosition({ 0,0,0 });
	Strength_Enemy2 = 0;
}

void EnemyThinking_Title::Action(Vector3 vec)
{
	Enemy1.Action(vec);
	Enemy2.Action(vec);
}



void EnemyThinking_Title::AddGun(M_Gun* gun)
{
	if (TotalGun >= 8) return;//GunObjectのサイズを超過するなら処理しない
	GunObject[TotalGun] = gun;
	TotalGun++;
}

void EnemyThinking_Title::RuleUpdate(uint64_t deltatime)
{
	Enemy1.Update(deltatime);
	Enemy1.Move();
	Enemy1.Timer(deltatime);
	Enemy1.Shot_Rule(deltatime);

	Enemy2.Update(deltatime);
	Enemy2.Move();
	Enemy2.Timer(deltatime);
	Enemy2.Shot_Rule(deltatime);

	//当たり判定処理
	for (int i = 0; i < Enemy1.GetBulletMaxnum(); i++) {

		bool col = GM31::GE::Collision::CollisionOBB(Enemy1.GetOBB(), Enemy2.GetOBB_Bullet(i));
		//索敵範囲内に弾丸があるかどうか
		bool inter = GM31::GE::Collision::CollisionSphereOBB_(Enemy1.GetShere(), Enemy2.GetOBB_Bullet(i));
		Enemy1.SetCollision(col);
		if (col) Enemy2.SetCollision_Bullet(i, col);
		if (inter) {
			Enemy1.SetAvoidance(inter);
			Enemy1.Stepavoidance(Enemy2.GetPosition(), true);
			//gun->SetCollision_Bullet(i, inter);
		}
	}
}

void EnemyThinking_Title::ThinkUpdate(uint64_t deltatime)
{	
	Enemy1.Update(deltatime);
	Enemy2.Update(deltatime);
	ThinkMove(deltatime);
	ThinkShot(deltatime);
	Enemy1.Timer(deltatime);
	Enemy2.Timer(deltatime);
	
	if (Enemy2.GetShotFlag())Strength_Enemy1 += 200;
	if (Enemy1.GetShotFlag())Strength_Enemy2 += 200;

}

void EnemyThinking_Title::ThinkMove(uint64_t deltatime) 
{
	ThinkMove_En1(deltatime);
	ThinkMove_En2(deltatime);
}

void EnemyThinking_Title::ThinkShot(uint64_t dt)
{
	//射撃は大技→通常射撃の順で行う
	if (Strength_Enemy1 > 1000 && Enemy1.GetFIRE() && !Enemy1.GetSpecialFlag()) {
		//Strength -= 40;
		Strength_Enemy1 -= 900;

		Enemy1.SetSpecialFlag(true);

		//Enemy.Shot(dt);

	}

	if (Strength_Enemy1 > 40 && Enemy1.GetFIRE()) {
		Strength_Enemy1 -= 40;
		//Strength -= 1000;

		Enemy1.Shot(dt);

	}

	//Enemy2
	//射撃は大技→通常射撃の順で行う
	if (Strength_Enemy2 > 1000 && Enemy2.GetFIRE() && !Enemy2.GetSpecialFlag()) {
		//Strength -= 40;
		Strength_Enemy2 -= 900;

		Enemy2.SetSpecialFlag(true);

		//Enemy.Shot(dt);

	}

	if (Strength_Enemy2 > 40 && Enemy2.GetFIRE()) {
		Strength_Enemy2 -= 40;
		//Strength -= 1000;

		Enemy2.Shot(dt);

	}
}

void EnemyThinking_Title::ThinkMove_En1(uint64_t deltatime)
{
	Vector3 pos_PL = Enemy2.GetPosition();
	Vector3 pos_EN = Enemy1.GetPosition();

	TimeLog += static_cast<float>(deltatime) / 1000;

	if (TimeLog > cooltime + (1000 / 60.0f)) {
		Vector3 localpos;
		localpos.x = Dot(pos_PL - pos_EN, Enemy2.GetRight());
		localpos.y = Dot(pos_PL - pos_EN, Enemy2.GetUp());
		localpos.z = Dot(pos_PL - pos_EN, Enemy2.GetForward());
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
	pos = Enemy2.GetPosition() + Enemy2.GetRight() * pos.x + Enemy2.GetUp() * pos.y + Enemy2.GetForward() * pos.z;
	Vector3 Targetpos = pos - Enemy2.GetPosition();//

	//当たり判定処理
	Vector3 minposition = { 0,0,0 };
	float range = 1000.0f;
	//銃からエネミーの弾丸に変更
	for (int i = 0; i < Enemy2.GetBulletMaxnum(); i++) {
		//弾丸を見てかわす
		bool inter = GM31::GE::Collision::CollisionSphereOBB_(Enemy1.GetShere(), Enemy2.GetOBB_Bullet(i));
		if (inter) {
			//かわしやすい方に移動してかわす
			Vector3 coppos_B = Enemy2.GetBulletpos(i);
			Vector3 coppos_E = Enemy1.GetPosition();
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
				minposition = Enemy2.GetBulletpos(i);
				range = rangedALL;
			}

			//gun->SetCollision_Bullet(i, inter);
		}
	}

	//範囲内に弾丸があった場合実行
	if (range != 1000.0f) {
		Vector3 localpos = minposition;
		localpos.x = Dot(localpos, Enemy1.GetRight());
		localpos.y = Dot(localpos, Enemy1.GetUp());
		localpos.z = Dot(localpos, Enemy1.GetForward());

		//最小座標に対しての右向きベクトルの取得
		Vector3 MoveVec = minposition - Enemy1.GetPosition();
		MoveVec.y = 0;//正規化前にy軸を切る
		MoveVec.Normalize();

		Vector3 up(0.0f, 1.0f, 0.0f); // ワールドの上方向
		Vector3 Targetright = MoveVec.Cross(up);
		Targetright.Normalize();      // 正規化して右向きベクトル完成

		if (localpos.x > 0) //ローカル座標に合わせて左右の判定をする
		{

			if (Strength_Enemy1 > 300 && range > 30)
			{

				Strength_Enemy1 -= 100;
				Enemy1.SetAvoidance(true);
				Enemy1.Stepavoidance(minposition, false);
			}
			else Enemy1.Move(Enemy1.GetPosition() - Targetright * 100);
		}
		else
		{
			if (Strength_Enemy1 > 300 && range > 30)
			{

				Strength_Enemy1 -= 100;
				Enemy1.SetAvoidance(true);
				Enemy1.Stepavoidance(minposition, true);
			}
			else Enemy1.Move(Enemy1.GetPosition() + Targetright * 100);

		}
		//Enemy.SetAvoidance(true);
		//Enemy.Stepavoidance(gun->GetPosition());
	}
	else Enemy1.Move(Targetpos);
}

void EnemyThinking_Title::ThinkMove_En2(uint64_t deltatime)
{
	Vector3 pos_PL = Enemy1.GetPosition();
	Vector3 pos_EN = Enemy2.GetPosition();

	
	//こっちの敵は固定値で距離を離すようにする
	Vector3 pos = Vector3(20, 0, -50);
	//距離の平均値出す

	//プレイヤーのローカル座標系の座標をワールド座標系へ復元
	pos = Enemy1.GetPosition() + Enemy1.GetRight() * pos.x + Enemy1.GetUp() * pos.y + Enemy1.GetForward() * pos.z;
	Vector3 Targetpos = pos - Enemy1.GetPosition();//

	//当たり判定処理
	Vector3 minposition = { 0,0,0 };
	float range = 1000.0f;
	//銃からエネミーの弾丸に変更
	for (int i = 0; i < Enemy1.GetBulletMaxnum(); i++) {
		//弾丸を見てかわす
		bool inter = GM31::GE::Collision::CollisionSphereOBB_(Enemy2.GetShere(), Enemy1.GetOBB_Bullet(i));
		if (inter) {
			//かわしやすい方に移動してかわす
			Vector3 coppos_B = Enemy1.GetBulletpos(i);
			Vector3 coppos_E = Enemy2.GetPosition();
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
				minposition = Enemy1.GetBulletpos(i);
				range = rangedALL;
			}

			//gun->SetCollision_Bullet(i, inter);
		}
	}

	//範囲内に弾丸があった場合実行
	if (range != 1000.0f) {
		Vector3 localpos = minposition;
		localpos.x = Dot(localpos, Enemy2.GetRight());
		localpos.y = Dot(localpos, Enemy2.GetUp());
		localpos.z = Dot(localpos, Enemy2.GetForward());

		//最小座標に対しての右向きベクトルの取得
		Vector3 MoveVec = minposition - Enemy2.GetPosition();
		MoveVec.y = 0;//正規化前にy軸を切る
		MoveVec.Normalize();

		Vector3 up(0.0f, 1.0f, 0.0f); // ワールドの上方向
		Vector3 Targetright = MoveVec.Cross(up);
		Targetright.Normalize();      // 正規化して右向きベクトル完成

		if (localpos.x > 0) //ローカル座標に合わせて左右の判定をする
		{

			if (Strength_Enemy2 > 300 && range > 30)
			{

				Strength_Enemy2 -= 100;
				Enemy2.SetAvoidance(true);
				Enemy2.Stepavoidance(minposition, false);
			}
			else Enemy2.Move(Enemy2.GetPosition() - Targetright * 100);
		}
		else
		{
			if (Strength_Enemy2 > 300 && range > 30)
			{

				Strength_Enemy2 -= 100;
				Enemy2.SetAvoidance(true);
				Enemy2.Stepavoidance(minposition, true);
			}
			else Enemy2.Move(Enemy2.GetPosition() + Targetright * 100);

		}
		//Enemy.SetAvoidance(true);
		//Enemy.Stepavoidance(gun->GetPosition());
	}
	else Enemy2.Move(Targetpos);
}