#include "system/commontypes.h"
#include "system/CStaticMesh.h"
#include "system/CStaticMeshRenderer.h"
#include "system/CDirectInput.h"
#include "system/DebugUI.h"
#include "Player.h"
#include "walls.h"

float VALUE_MOVE_MODEL = 2.0f;						// キー入力時の移動量
float VALUE_ROTATE_MODEL = PI * 0.02f;				// キー入力時の回転量
float RATE_ROTATE_MODEL = 0.40f;					// １フレーム当たりの回転割合
float RATE_MOVE_MODEL = 0.20f;						// １フレーム当たりの減衰割合

// debug用
static Vector3 g_rotation = Vector3(0.0f, 0.0f, 0.0f);
static Vector3 g_position = Vector3(0.0f, 0.0f, 0.0f);
static Vector3 g_scale = Vector3(1.0f, 1.0f, 1.0f);

static void DebugPlayerMoveParameter() {

	ImGui::Begin("Debug Player Move Parameter");

	ImGui::SliderFloat("VALUE_MOVE_MODEL", &VALUE_MOVE_MODEL, 0.01f, 3.0f);
	ImGui::SliderFloat("VALUE_ROTATE_MODEL", &VALUE_ROTATE_MODEL, 0.01f, PI / 4.0f);
	ImGui::SliderFloat("RATE_ROTATE_MODEL", &RATE_ROTATE_MODEL, 0.0f, 1.0f);
	ImGui::SliderFloat("RATE_MOVE_MODEL", &RATE_MOVE_MODEL, 0.0f, 1.0f);

	ImGui::Text("ROTATION %f %f %f", g_rotation.x, g_rotation.y, g_rotation.z);
	ImGui::Text("POSITION %f %f %f", g_position.x, g_position.y, g_position.z);
	ImGui::Text("SCALE %f %f %f", g_scale.x, g_scale.y, g_scale.z);

	ImGui::End();
}

void Player::Init()
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

void Player::Update()
{
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
		{// 左前移動

			float radian;
			radian = PI * 0.75f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 左後移動

			float radian;
			radian = PI * 0.25f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Destrot.y = radian;
		}
		else
		{// 左移動

			float radian;
			radian = PI * 0.50f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Destrot.y = radian;
		}
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D))
	{
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			// 右前移動

			float radian;
			radian = -PI * 0.75f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 右後移動
			float radian;
			radian = -PI * 0.25f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Destrot.y = radian;
		}
		else
		{// 右移動

			float radian;
			radian = -PI * 0.50f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Destrot.y = radian;
		}
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
	{// 前移動
		float radian;
		radian = PI;

		m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
		m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

		// 目標角度をセット
		m_Destrot.y = PI;
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
	{// 後移動
		float radian;
		radian = 0.0f;

		m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
		m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

		// 目標角度をセット
		m_Destrot.y = 0.0f;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT))
	{// 左回転
		m_Destrot.y = m_Rotation.y - VALUE_ROTATE_MODEL;
		if (m_Destrot.y < -PI)
		{
			m_Destrot.y += PI * 2.0f;
		}
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT))
	{// 右回転
		m_Destrot.y = m_Rotation.y + VALUE_ROTATE_MODEL;
		if (m_Destrot.y > PI)
		{
			m_Destrot.y -= PI * 2.0f;
		}
	}

	// 目標角度と現在角度との差分を求める
	float diffrot = m_Destrot.y - m_Rotation.y;
	if (diffrot > PI)
	{
		diffrot -= PI * 2.0f;
	}
	if (diffrot < -PI)
	{
		diffrot += PI * 2.0f;
	}

	// 比率計算
	m_Rotation.y += diffrot * RATE_ROTATE_MODEL;
	if (m_Rotation.y > PI)
	{
		m_Rotation.y -= PI * 2.0f;
	}
	if (m_Rotation.y < -PI)
	{
		m_Rotation.y += PI * 2.0f;
	}


	/// 位置移動
	m_Position += m_Move;

	// 移動量に慣性をかける(減速率)
	m_Move += -m_Move * RATE_MOVE_MODEL;

	// リセット
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RETURN))
	{// リセット
		m_Position = Vector3(0.0f, 0.0f, 0.0f);
		m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	}
}

void Player::Update2()
{

	m_Move = Vector3(0, 0, 0);			//押されたときのみ動く
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
		{// 左前移動

			float radian;
			radian = PI * 0.75f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Rotation.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 左後移動

			float radian;
			radian = PI * 0.25f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Rotation.y = radian;
		}
		else
		{// 左移動

			float radian;
			radian = PI * 0.50f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Rotation.y = radian;
		}
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D))
	{
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			// 右前移動

			float radian;
			radian = -PI * 0.75f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Rotation.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 右後移動
			float radian;
			radian = -PI * 0.25f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Rotation.y = radian;
		}
		else
		{// 右移動

			float radian;
			radian = -PI * 0.50f;

			m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_Rotation.y = radian;
		}
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
	{// 前移動
		float radian;
		radian = PI;

		m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
		m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

		// 目標角度をセット
		m_Rotation.y = radian;
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
	{// 後移動
		float radian;
		radian = 0.0f;

		m_Move.x -= sinf(radian) * VALUE_MOVE_MODEL;
		m_Move.z -= cosf(radian) * VALUE_MOVE_MODEL;

		// 目標角度をセット
		m_Rotation.y = radian;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT))
	{// 左回転
		m_Destrot.y = m_Rotation.y - VALUE_ROTATE_MODEL;
		if (m_Destrot.y < -PI)
		{
			m_Destrot.y += PI * 2.0f;
		}
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT))
	{// 右回転
		m_Destrot.y = m_Rotation.y + VALUE_ROTATE_MODEL;
		if (m_Destrot.y > PI)
		{
			m_Destrot.y -= PI * 2.0f;
		}
	}

	/// 位置移動
	m_Position += m_Move;

	// リセット
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RETURN))
	{// リセット
		m_Position = Vector3(0.0f, 0.0f, 0.0f);
		m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	}
}

void Player::Draw()
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

	// デバッグ用のグローバル変数に値をセット
	g_position = m_Position;
	g_rotation = m_Rotation;
	g_scale = m_Scale;
}

void Player::Dispose()
{

}