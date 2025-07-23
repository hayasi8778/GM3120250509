#include "M_Rock.h"

void M_Rock::Init()
{

	// モデルの初期化
	m_mesh.Load(
		"assets/model/car000.x",				// モデル名
		"assets/model/");						// テクスチャのパス

	

	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	m_shader.Create(
		"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	//	DebugUI::RedistDebugFunction(DebugPlayerMoveParameter);

}

void M_Rock::Update()
{

}

void M_Rock::Draw()
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
}

void M_Rock::Dispose()
{

}