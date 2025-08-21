#include "Player_Mec.h"


void M_Player::Init()
{
	//属性
	Attribute = PLAYER;

	//プレイヤーなので接触フラグは最初からon
	adhesioing = true;
	

	//ロボットモデル
	m_mesh.Load(
		"assets/model/Mec/MecBone.fbx",				// モデル名
		"assets/model/Mec/");						// テクスチャのパス

	////ロボットモデル
	//m_mesh.Load(
	//	"assets/model/Mec/NeoMecBone.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	//m_mesh.Load(
	//	"assets/model/Mec/NeoMecBone4.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	//テスト用のモデル
	//m_mesh.Load(
	//	"assets/model/Tesmodel/man.fbx",				// モデル名
	//	"assets/model/Tesmodel/");						// テクスチャのパス


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


	//デバック用GUI一式
	// BOXのSRTの設定用
	DebugUI::RedistDebugFunction([this]() {
		Debug_Player();
		});

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
	//衝突判定と無敵時間の処理
	if (col)
	{
		Invincibility_time += static_cast<float>(deltatime) / 1000;

		if (Invincibility_time > 1000)
		{
			HP--;
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

	

	
}

	void M_Player::Draw()
{
		//姿勢の補完をここでする
		m_Rotation.x += 1.55;
		m_Rotation.y += 1.55;

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	m_shader.SetGPU();
	
	if (HP > 0) 
	{
		if (DrawModel)m_meshrenderer.Draw();

		if (DrawBone)m_meshrenderer.DrawWithBones(srt, { 1.0f, 1.0f, 0.0f });
	}
	

	//// デバッグ用のグローバル変数に値をセット
	//g_position = m_Position;
	//g_rotation = m_Rotation;
	//g_scale = m_Scale;

	for (int i = 0; i < 20; i++)
	{
		m_bullet[i].Draw();
	}

	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;


	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	if (col) {
		m_shapecube_col->Draw(transmtx, { 0.6,0.0,0.0,0.5 });
	}
	else 
	{
		m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });
	}
	

	m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;
}

void M_Player::Dispose()
{

}

void M_Player::Adhesioing()
{

}

void M_Player::Action(Vector3 vec)
{
	Burst = true;

	FullBurst();
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
	//姿勢補完分
	m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;

	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	Vector3 returnpos = m_meshrenderer.LogBoneWorldPosition("Conect", srt);

	m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;

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

//バグが起きた時のために一応残しておく
//void M_Player::ModelAABB(aiVector3D& outMin, aiVector3D& outMax)
//{
//	m_meshrenderer.ComputeModelAABB(outMin, outMax);
//}
