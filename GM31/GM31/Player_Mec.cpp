#include "Player_Mec.h"
#include <iostream>

void M_Player::Init()
{
	//属性
	Attribute = PLAYER;

	//プレイヤーなので接触フラグは最初からon
	adhesioing = true;

	//ロボットモデル(胴体)
	m_mesh.Load(
		"assets/model/Mec/MecBone_Body.fbx",				// モデル名
		"assets/model/Mec/");						// テクスチャのパス

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
	m_shader.Create(
		"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	//	DebugUI::RedistDebugFunction(DebugPlayerMoveParameter);

	//スケール調整
	SetScale({ 1.0f,1.0f,1.0f });

	//位置補正
	m_Position.y += 9;

	for (int i = 0; i < 20; i++) 
	{
		m_bullet[i].Init();
	}

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

	//衝突判定と無敵時間の処理
	if (col)
	{
		Invincibility_time += static_cast<float>(deltatime) / 1000;

		if (Invincibility_time > 1000)
		{
			col = false;

			Invincibility_time = 0;
		}

	}

	if (Burst) //フルバーストするならの処理
	{
		time += static_cast<float>(deltatime) / 1000;
		if (time > 100) 
		{
			FullBurst();
			if (bulletcur > 19) {
				Burst = false;
				bulletcur = 0;
			}
			time = 0;
		}

	}

	for (int i = 0; i < 20; i++)
	{
		m_bullet[i].Update(deltatime);
	}

	head.Update(deltatime);
	leftarm.Update(deltatime);
	rightarm.Update(deltatime);
	leftfeet.Update(deltatime);
	rightfeet.Update(deltatime);

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

	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	head.SetRotation(srt.rot);
	//leftarm.SetRotation(srt.rot);
	rightarm.SetRotation(srt.rot);
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

	m_shader.SetGPU();//これ最終的に外に持っていきたい

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

	for (int i = 0; i < 20; i++)
	{
		//m_bullet[i].Draw();
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

	if (col) {
		m_shapecube_col->Draw(transmtx, { 0.6,0.0,0.0,0.5 });
	}
	else 
	{
		m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });
	}
	
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

	DoublePistol++;
	if (DoublePistol > 2) 
	{
		DoublePistol = 0;
	}

	head.Action(vec);
	if(DoublePistol !=1)leftarm.Action(vec);
	if (DoublePistol != 0)rightarm.Action(vec);
	leftfeet.Action(vec);
	rightfeet.Action(vec);
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

void M_Player::Reset() 
{
	HP = MaxHP;

	head.Reset();
	leftarm.Reset();
	rightarm.Reset();
	leftfeet.Reset();
	rightfeet.Reset();
}

GM31::GE::Collision::BoundingBoxOBB M_Player::GetOBB_Bullet(int i)
{
	return m_bullet[i].GetOBB();
}

Vector3 M_Player::GetForward()
{
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	//新しい弾を作る
	Matrix4x4 world = srt.GetMatrix();
	Vector3 Forward = world.Forward();
	return Forward;
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

void M_Player::FullBurst()
{
	if (!Target) return;
	
	SRT srt = GetSRT();

	srt.rot.x += 1.5;
	Matrix4x4 world = srt.GetMatrix();
	Vector3 forward = world.Forward();
	forward.Normalize();
	forward *= 3.0f;

	m_bullet[bulletcur].SetForward(forward);

	////前向き行列取ってから姿勢補完する
	////姿勢補完分
	//srt.rot.x += 1.55;
	//srt.rot.y += 1.55;
	//Vector3 bulletpos = m_meshrenderer.LogBoneWorldPosition("Shot", srt);


	m_bullet[bulletcur].SetScale(Vector3(1, 1, 1));
	m_bullet[bulletcur].SetRotation(srt.rot);
	m_bullet[bulletcur].SetPosition(srt.pos);
	m_bullet[bulletcur].Setinduction(1000, forward);
	m_bullet[bulletcur].SetTarget(Target);

	bulletcur++;
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

void M_Player::SetCollision_Bullet(int num, bool col)
{
	std::cout << "[SetCollision_Bullet] num=" << num
		<< " col=" << col
		<< " time=" << std::chrono::steady_clock::now().time_since_epoch().count()
		<< std::endl;

	m_bullet[num].SetCol(col);
}

//バグが起きた時のために一応残しておく
//void M_Player::ModelAABB(aiVector3D& outMin, aiVector3D& outMax)
//{
//	m_meshrenderer.ComputeModelAABB(outMin, outMax);
//}
