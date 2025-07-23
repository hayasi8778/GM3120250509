#include "Player_Mec.h"

void M_Player::Init()
{
	//プレイヤーなので接触フラグは最初からon
	Adhesioing = true;

	// モデルの初期化
	//m_mesh.Load(
	//	"assets/model/car000.x",				// モデル名
	//	"assets/model/");						// テクスチャのパス

	////ロボットモデル
	//m_mesh.Load(
	//	"assets/model/Mec/MecArm_LeflCop.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	//テスト用のモデル
	m_mesh.Load(
		"assets/model/Tesmodel/man.fbx",				// モデル名
		"assets/model/Tesmodel/");						// テクスチャのパス


	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	m_shader.Create(
		"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	//	DebugUI::RedistDebugFunction(DebugPlayerMoveParameter);

	//スケール調整
	//SetScale({ 0.1f,0.1f,0.1f });



}

void M_Player::Update()
{

}

	void M_Player::Draw()
{
	// SRT情報作成
	SRT srt;
	srt.pos = m_Position;			// 位置
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.scale = m_Scale;			// 拡縮

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	m_shader.SetGPU();

	m_meshrenderer.Draw();

	//// デバッグ用のグローバル変数に値をセット
	//g_position = m_Position;
	//g_rotation = m_Rotation;
	//g_scale = m_Scale;
	

	m_mesh.DrawWithBones(srt , {1.0f,1.0f,1.0f});
}

void M_Player::Dispose()
{

}