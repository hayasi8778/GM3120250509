#include "Player_Mec.h"


void M_Player::Init()
{
	//プレイヤーなので接触フラグは最初からon
	Adhesioing = true;

	m_bullet.Init();

	//ロボットモデル
	//m_mesh.Load(
	//	"assets/model/Mec/MecBone.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	//ロボットモデル
	m_mesh.Load(
		"assets/model/Mec/NeoMecBone.fbx",				// モデル名
		"assets/model/Mec/");						// テクスチャのパス

	//m_mesh.Load(
	//	"assets/model/Mec/NeoMecBone3.fbx",				// モデル名
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


	//m_Rotation.x += 1;
	//m_Rotation.y += 1;

	//デバック用GUI一式
	// BOXのSRTの設定用
	DebugUI::RedistDebugFunction([this]() {
		Debug_Player();
		});

}

void M_Player::Update()
{
	//m_Rotation.x += 0.01;

	m_bullet.Update();
}

	void M_Player::Draw()
{
	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	m_shader.SetGPU();
	
	m_meshrenderer.Draw();

	m_meshrenderer.DrawWithBones(srt, { 1.0f, 1.0f, 0.0f });

	//// デバッグ用のグローバル変数に値をセット
	//g_position = m_Position;
	//g_rotation = m_Rotation;
	//g_scale = m_Scale;
	
	m_bullet.Draw();
}

void M_Player::Dispose()
{

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
