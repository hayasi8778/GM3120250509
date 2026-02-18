#include "Player_Mec.h"
#include <iostream>

void M_Player::Init()
{
	//属性
	Attribute = PLAYER;
	MyType = CHARACTER;
	//プレイヤーなので接触フラグは最初からon
	adhesioing = true;

	//ロボットモデル(胴体)
	m_mesh.Load(
		"assets/model/Mec/MecBone_Body.fbx",				// モデル名
		"assets/model/Mec/");						// テクスチャのパス

	//m_mesh.Load(
	//	"assets/model/Mec/MecBone_Body_white.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	//ロボットモデル(左腕)
	//m_mesh.Load(
	//	"assets/model/Mec/MecBone_LeftArm_TestModel.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	////ロボットモデル(右腕)
	//m_mesh.Load(
	//	"assets/model/Mec/MecBone_RightArm.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	////ロボットモデル(左足)
	//m_mesh.Load(
	//	"assets/model/Mec/MecBone_LeftFeet.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	////ロボットモデル(右足)
	//m_mesh.Load(
	//	"assets/model/Mec/MecBone_RightFeet.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス


	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	m_Rotation = m_meshrenderer.GetModelRot();

	// シェーダーの初期化
	//m_shader.Create(
	//	"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
	//	"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	//	DebugUI::RedistDebugFunction(DebugPlayerMoveParameter);

	for (int i = 0; i < MaxBullets; i++)
	{
		m_bullets[i].Init();
	}

	//スケール調整
	SetScale({ 1.0f,1.0f,1.0f });

	//位置補正
	m_Position.y += 9;


	head.Init();
	leftarm.Init();
	rightarm.Init();
	leftfeet.Init();
	rightfeet.Init();

	//デバック用GUI一式
	//// BOXのSRTの設定用
	//DebugUI::RedistDebugFunction([this]() {
	//	Debug_Player();
	//	});

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);


}

void M_Player::Update(uint64_t deltatime)
{

	//m_Rotation.y += 0.1;
	//m_Rotation.z += 0.1;

	//被弾しているならHPを減らす
	if (Damage != 0) {
		HP--;
		Damage--;
		if (Damage < 0)Damage = 0;
	}

	float timeD = static_cast<float>(deltatime) / 1000;

	//衝突判定と無敵時間の処理
	if (col)
	{
		Invincibility_time += timeD;

		if (Invincibility_time > 1000)
		{
			col = false;

			Invincibility_time = 0;
		}

	}


	if (ActionCool != 0) {
		ActionCool -= timeD;
		if (ActionCool < 0)ActionCool = 0;
	}


	if (!Burst && BurstCoolTime != 0) {
		BurstCoolTime -= timeD;
		if (BurstCoolTime < 0) 
		{ 
			BurstCoolTime = 0; 

		}
	}

	//攻撃の間隔
	if (ActionInterval < 5000.0f) {
		ActionInterval += timeD;
		if (ActionInterval > 5000.0f) ActionInterval = 5000.0f;
	}

	if (Target != nullptr) {
		//ターゲットの方向く
		Vector3 TargetForward = (m_Position - *Target);

		TargetForward.Normalize();

		// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
		float yaw = atan2f(TargetForward.x, TargetForward.z);
		float pitch = atan2f(-TargetForward.y,
			sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

		// 4. Roll は今回は固定 0
		SetRotation_PL({ 0.0f,yaw, 0.0f });
	}
	
	//必殺技のフラグ立ってるなら使用する
	if (Burst) SpecialAttack(deltatime);

	for (int i = 0; i < MaxBullets; i++)
	{
		m_bullets[i].Update(deltatime);
	}
	

	head.Update(deltatime);
	leftarm.Update(deltatime);
	rightarm.Update(deltatime);
	leftfeet.Update(deltatime);
	rightfeet.Update(deltatime);

	Matrix4x4 rotX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 反転したいならここで反転
	Matrix4x4 rot = rotX * rotY * rotZ;

	// 方向ベクトルは回転行列からのみ抽出
	Right_vec = { rot._11, rot._12, rot._13 };
	Up_vec = { rot._21, rot._22, rot._23 };
	Forward_vec = { rot._31, rot._32, rot._33 };

	// 必要ならここで反転
	Forward_vec *= -1.0f;  // ← モデルが反転しているならこれでOK
	Right_vec *= -1.0f;


	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	head.SetRotation(srt.rot);
	//leftarm.SetRotation(srt.rot);
	//rightarm.SetRotation(srt.rot);
	leftfeet.SetRotation(srt.rot);
	rightfeet.SetRotation(srt.rot);
	//姿勢の補完をここでする
	srt.rot.x += 1.55;
	srt.rot.y += 1.55;

	head.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_Neck", srt));
	leftarm.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_LeftArm", srt));
	rightarm.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_RightArm", srt));
	leftfeet.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_LeftFeet", srt));
	rightfeet.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_RightFeet", srt));
	
}

void M_Player::LateUpdate(uint64_t deltatime) 
{
	Shot = false;//射撃フラグ切る
	BurstFlag = false;
	head.LateUpdate(deltatime);
	leftarm.LateUpdate(deltatime);
	rightarm.LateUpdate(deltatime);
	leftfeet.LateUpdate(deltatime);
	rightfeet.LateUpdate(deltatime);
}

void M_Player::Draw()
{

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	//姿勢保管
	srt.rot.x += 1.55;
	srt.rot.y += 1.55;

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	//m_shader.SetGPU();//これ最終的に外に持っていきたい

	if (HP > 0) 
	{
		if (DrawModel)m_meshrenderer.Draw();

		if (DrawBone)m_meshrenderer.DrawWithBones(srt, { 1.0f, 1.0f, 0.0f });
	}
	
	//プレイヤーの階層型モデル
	head.Draw();
	leftarm.Draw();
	rightarm.Draw();
	leftfeet.Draw();
	rightfeet.Draw();

	//// デバッグ用のグローバル変数に値をセット
	//g_position = m_Position;
	//g_rotation = m_Rotation;
	//g_scale = m_Scale;

	for (int i = 0; i < MaxBullets; i++)
	{
		m_bullets[i].Draw();
	}

	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(srt.rot.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(srt.rot.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(srt.rot.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	Vector3 poscop = m_Position;

	m_Position += Up_vec * 1;
	m_Position += Forward_vec * 0.3;
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);
	m_Position = poscop;

	//当たり判定のボックス表示
	/*if (col) {
		m_shapecube_col->Draw(transmtx, { 0.6,0.0,0.0,0.5 });
	}
	else 
	{
		m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });
	}*/
	
}

void M_Player::Dispose()
{

}

void M_Player::Adhesioing()
{

}

void M_Player::Action(Vector3 vec)
{
	/*Burst = true;
	FullBurst();*/

	if (ActionCool != 0)return;

	DoublePistol++;
	if (DoublePistol > 1) 
	{
		DoublePistol = 0;
	}

	head.Action(vec);
	if (DoublePistol != 1) 
	{ 
		leftarm.Action(*Target); 
	}
	if (DoublePistol != 0)
	{
		rightarm.Action(*Target);
	}
	leftfeet.Action(vec);
	rightfeet.Action(vec);

	Shot = true;//射撃フラグを付ける
	ActionCool = 300;//0.3秒遅延
	ActionInterval = 0.0f;
}

GM31::GE::Collision::BoundingBoxOBB M_Player::GetOBB()
{
	GM31::GE::Collision::BoundingBoxOBB obb;

	//姿勢の補完をここでする
	m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;

	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		m_Position,				// 中心座標（ワールド）
		Width,					// 幅
		Height,					// 高さ
		Depth);					// 奥行

	//姿勢の補完をここでする
	m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;

	return obb;
}

GM31::GE::Collision::BoundingBoxOBB M_Player::GetOBB_Bullet(int num)
{
	if (num < MaxBullets)
	{
		return  m_bullets[num].GetOBB();
	}

	GM31::GE::Collision::BoundingBoxOBB colbox;

	return colbox;
}

void M_Player::SetCollision_Bullet(int num, bool col)
{
	m_bullets[num].SetCol(col);
}

void M_Player::Reset() 
{
	HP = MaxHP;
	Damage = 0;

	m_Position = { 0.0f, 9.0f,0.0f };

	head.Reset();
	leftarm.Reset();
	rightarm.Reset();
	leftfeet.Reset();
	rightfeet.Reset();

	//特殊攻撃関連の初期化
	for (int i = 0; i < MaxBullets; i++)
	{
		m_bullets[i].Reset();
	}
	Burst = false;
	BurstCoolTime = 0;
	BurstCount = 0;
	bulletcur = 0;

	//被ダメ関係の初期化もしておく
	Damage = 0;
	Invincibility_time = 0.0f;
}

int M_Player::GetShaderNum()
{
	return 0;
}

Vector3 M_Player::GetForward()
{
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	//前向きベクトルをとって返す
	Matrix4x4 world = srt.GetMatrix();
	Vector3 Forward = world.Forward();
	return Forward;
}

Vector3 M_Player::GetRight()
{
	return Right_vec;
}

Vector3 M_Player::GetUp()
{
	return Up_vec;
}

void M_Player::SpecialAttack(uint64_t deltatime) 
{
	//Burst = false;
	float time = static_cast<float>(deltatime) / 1000;
	BurstCoolTime += time;
	if (BurstCoolTime > 5000) {
		Burst = false;
		BurstCoolTime = 0;
		bulletcur = 0;
	}

	if (bulletcur == MaxBullets)  return;//bulletcur = 0;

	BurstCount += time;
	if (BurstCount > 100) {
		BurstCount = 0;
	}
	else return;

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	//pb->SetForward(forward);
	//m_bullets[bulletcur].SetForward(Up_vec);
	m_bullets[bulletcur].Setinduction(2000.0f, Up_vec);
	m_bullets[bulletcur].SetTarget(Target);

	//pb->SetScale(Vector3(1, 1, 1));
	//pb->SetRotation(m_Rotation);
	//pb->SetPosition(bulletpos);

	//m_bullets[bulletnum].SetScale(Vector3(1, 1, 1));
	//m_bullets[bulletcur].SetScale(Vector3(0.5, 0.5, 0.5));
	m_bullets[bulletcur].SetRotation(m_Rotation);
	m_bullets[bulletcur].SetPosition(m_Position);

	bulletcur++;
}

void M_Player::SetTarget(Vector3* vec)//対象の座標を保存しつつその方向を向く
{
	Target = vec;

	//ターゲットの方向く
	Vector3 TargetForward = (m_Position - *Target);

	TargetForward.Normalize();

	// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
	float yaw = atan2f(TargetForward.x, TargetForward.z);
	float pitch = atan2f(-TargetForward.y,
		sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

	// 4. Roll は今回は固定 0
	m_Rotation = Vector3{ 0.0f,yaw, 0.0f };

	// 方向ベクトル作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	// 方向ベクトル 抽出
	Right_vec = { transmtx._11, transmtx._12, transmtx._13 };
	Right_vec.Normalize();
	Up_vec = { transmtx._21, transmtx._22, transmtx._23 };
	Up_vec.Normalize();
	Forward_vec = { transmtx._31, transmtx._32, transmtx._33 };
	Forward_vec.Normalize();
}


void M_Player::Debug_Player()
{
	ImGui::Begin("debug Player SRT");

	ImGui::Text("Player");
	ImGui::SliderFloat3((std::string(" Player") + std::string(" pos")).c_str(), &m_Position.x, -100, 100);
	ImGui::SliderFloat3((std::string(" Player") + std::string(" rot")).c_str(), &m_Rotation.x, -100, 100);
	ImGui::SliderFloat3((std::string(" Player") + std::string(" size")).c_str(), &m_Scale.x, 1, 100);

	// カメラの位置を極座標からデカルト座標に変換
	ImGui::End();
}

Vector3 M_Player::ConectPos() 
{

	Vector3 rotcop = m_Rotation;

	Vector3 returnpos = Vector3::Zero;

	//姿勢補完分
	rotcop.x += 1.55;
	rotcop.y += 1.55;

	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = rotcop;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	returnpos = m_meshrenderer.LogBoneWorldPosition("Conect", srt);

	//returnpos =  m_meshrenderer.LogBoneWorldPosition("Hand", srt);

	if (returnpos != Vector3::Zero)return returnpos;

	returnpos = head.Conectpos("Conect");

	if (returnpos != Vector3::Zero)return returnpos;

	returnpos = leftarm.Conectpos("Hand");

	if (returnpos != Vector3::Zero)return returnpos;

	returnpos = rightarm.Conectpos("Hand");

	if (returnpos != Vector3::Zero)return returnpos;

	returnpos = leftfeet.Conectpos("Conect");

	if (returnpos != Vector3::Zero)return returnpos;

	returnpos = rightfeet.Conectpos("Conect");

	if (returnpos != Vector3::Zero)return returnpos;

	return returnpos;
	
}

Vector3 M_Player::ConectPos(int i)
{

	Vector3 returnpos = Vector3::Zero;
	Vector3 rotcop = m_Rotation;
	SRT srt;
	switch (i)
	{
	case 0:

		//姿勢補完分
		rotcop.x += 1.55;
		rotcop.y += 1.55;

		srt.scale = m_Scale;			// 拡縮
		srt.rot = rotcop;			// 姿勢	srt.pos = m_Position;
		srt.pos = m_Position;			// 位置

		returnpos = m_meshrenderer.LogBoneWorldPosition("Conect", srt);
		
		break;

	case 1://頭
		returnpos = head.Conectpos("Conect");
		break;

	case 2://左腕
		returnpos = leftarm.Conectpos("Hand");
		//returnpos = leftarm.Conectpos("Conect");
		break;

	case 3://右腕
		returnpos = rightarm.Conectpos("Hand");
		break;

	case 4://左足
		returnpos = leftfeet.Conectpos("Conect");
		break;

	case 5://右足
		returnpos = rightfeet.Conectpos("Conect");
		break;

	default:
		break;
	}

	return returnpos;

}

void M_Player::SetRotation_PL(Vector3 rot)
{
	SRT srt;
	srt.scale = m_Scale;
	srt.rot = rot;
	srt.pos = m_Position;

	SetRotation(rot);
	head.SetRotation(rot);
	if (Target) {
		leftarm.Rockon(*Target);
		rightarm.Rockon(*Target);
	}
	else {
		leftarm.SetRotation(rot);
		rightarm.SetRotation(rot);
	}
	
	leftfeet.SetRotation(rot);
	rightfeet.SetRotation(rot);

	srt.rot.x += 1.55;
	srt.rot.y += 1.55;

	//プレイヤーを構成するパーツの位置を再調整
	head.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_Neck", srt));
	leftarm.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_LeftArm", srt));
	rightarm.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_RightArm", srt));
	leftfeet.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_LeftFeet", srt));
	rightfeet.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_RightFeet", srt));
}

bool M_Player::Connectable(int ConnectionPoint)
{
	switch (ConnectionPoint) {
	case 0://頭
		if (!HeadSet) return true;
		break;
	case 1://胴体
		if (!BodySet) return true;
		break;
	case 2://左腕
		if (!LeftArmSet) return true;
		break;
	case 3://右腕
		if (!RightArmSet) return true;
		break;
	case 4://左足
		if (!LeftArmSet) return true;
		break;
	case 5://右足
		if (!RightArmSet) return true;
		break;
	}
	//範囲外か既に装着済みならfalseで返す
	return false;
}

bool M_Player::Collision_PL(GM31::GE::Collision::BoundingBoxOBB colobb)
{
	//プレイヤーを構成する要素全てと判定取ってぶつかってたらその時点でtrue返す
	if (GM31::GE::Collision::CollisionOBB(GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(head.GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(leftarm.GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(rightarm.GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(leftfeet.GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(rightfeet.GetOBB(), colobb)) return true;
	return false;
}

int M_Player::Canconect(Object* obj)
{
	//接続可能な中で一番若い数を返す
	if (obj->CanConnectable(0) && head.CanConect()) return 0;
	if (obj->CanConnectable(2) && leftarm.CanConect()) return 2;
	if (obj->CanConnectable(3) && rightarm.CanConect()) return 3;
	if (obj->CanConnectable(4) && leftfeet.CanConect()) return 4;
	if (obj->CanConnectable(5) && rightfeet.CanConect()) return 5;

	return 100;
}

void M_Player::Conect(int Adhesinom, Object* obj)
{
	switch (Adhesinom)
	{
	case 0:
		head.Conect(obj);
		break;
	case 1:
		Connectableobject = obj;
		//接続フラグをonにして接続時の処理を通す
		obj->SetAdhesioing(true);
		obj->Adhesioing();
		break;
	case 2:
		leftarm.Conect(obj);
		break;
	case 3:
		rightarm.Conect(obj);
		break;
	case 4:
		leftfeet.Conect(obj);
		break;
	case 5:
		rightfeet.Conect(obj);
		break;
	default:
		break;
	}
}

void M_Player::Release(int Adhesinom)
{
	switch (Adhesinom)
	{
	case 0:
		head.Release();
		break;
	case 1:
		Connectableobject = nullptr;
		break;
	case 2:
		leftarm.Release();
		break;
	case 3:
		rightarm.Release();
		break;
	case 4:
		leftfeet.Release();
		break;
	case 5:
		rightfeet.Release();
		break;
	}
}

void M_Player::ReleaseALL()
{
		head.Release();
		if (Connectableobject) Connectableobject->SetAdhesioing(false);
		Connectableobject = nullptr;
		leftarm.Release();
		rightarm.Release();
		leftfeet.Release();
		rightfeet.Release();

}

//バグが起きた時のために一応残しておく
//void M_Player::ModelAABB(aiVector3D& outMin, aiVector3D& outMax)
//{
//	m_meshrenderer.ComputeModelAABB(outMin, outMax);
//}
