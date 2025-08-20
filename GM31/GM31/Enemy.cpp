#include "system/commontypes.h"
#include "system/CStaticMesh.h"
#include "system/CStaticMeshRenderer.h"
#include "system/CDirectInput.h"
#include "Enemy_Default.h"
#include "Player.h"
#include "EnemyLookatScene.h"
#include "Enemy_Rotation_Random.h"

void Enemy::Init()
{

	m_Position = Vector3(0.0f, 0.0f, 0.0f);
	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	m_Scale = Vector3(1.0f, 1.0f, 1.0f);
}

void Enemy::Update()
{
	Player* p;

	p = ((EnemyLookatScene*)(m_ownerscene))->GetPlayer();	// プレイヤの取得
	if (WallSide) 
	{
		Matrix4x4 mtx = Matrix4x4::CreateRotationY(m_Rotation.y);	// 目標角度を現在の角度に更新
		Vector3 forward = mtx.Forward();	// 前方ベクトルを取得
		m_Move = forward * m_speed;	// 前方ベクトルにスピードを掛けて移動量を計算

		//ここから移動距離の判定
		 //現在位置と旧位置の差分ベクトル
		Vector3 delta = m_Position - Oldpos;

		 //forward が単位長でない場合は正規化
		forward.Normalize();  // in-place で長さ1に

		//delta.Dot(forward);

		 //旧位置→現在位置ベクトルを forward 方向に射影した距離
		float forwardDistance = delta.Dot(forward);

		if (forwardDistance > Wall_Width)
		{
			WallSide = false;
		}
	}
	else 
	{
		// プレイヤの座標を取得
		Vector3 playerpos = p->GetPosition();

		// atan2を使用して角度を求める
		m_Destrot.y = atan2f(-(playerpos.x - m_Position.x), -(playerpos.z - m_Position.z));

		// 現在の向きとの差分を計算する
		float fDiffRotY = m_Destrot.y - m_Rotation.y;

		// 補正（－１８０～１８０の範囲）
		if (fDiffRotY > PI)
		{
			fDiffRotY -= PI * 2.0f;
		}
		if (fDiffRotY < -PI)
		{
			fDiffRotY += PI * 2.0f;
		}

		// 回転角度計算
		m_Rotation.y += fDiffRotY * RATE_ROTATE_ENEMY;
		if (m_Rotation.y > PI)
		{
			m_Rotation.y -= PI * 2.0f;
		}
		if (m_Rotation.y < -PI)
		{
			m_Rotation.y += PI * 2.0f;
		}

		Matrix4x4 mtx = Matrix4x4::CreateRotationY(m_Rotation.y);	// 目標角度を現在の角度に更新
		Vector3 forward = mtx.Forward();	// 前方ベクトルを取得

		m_Move = forward * m_speed;	// 前方ベクトルにスピードを掛けて移動量を計算

		
	}

	/// 位置移動
	m_Position += m_Move;

	// 移動量に慣性をかける(減速率)
	m_Move += -m_Move * RATE_MOVE_MODEL;
	
}


void Enemy::Draw()
{
	SRT srt;

	// SRT情報作成
	srt.pos = m_Position;			// 位置
	srt.rot = m_Rotation;			// 姿勢
	srt.scale = m_Scale;			// 拡縮

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	m_StaticMeshRenderer->Draw();
}

void Enemy::Dispose()
{

}

void Enemy::Remove(Vector3 Rot, float Wall_Si)//移動前に戻す処理
{
	//// 移動速度の逆数を足す
	//m_Position -= m_Move * 2;

	//m_Rotation.y += 0.5f;

	Rot.y += 5.0f + RandomFloat(0.0f, 1.5f);//ある程度角度にランダム性を持たせる

	WallSide = true;

	Wall_Width = Wall_Si;

	m_Rotation = Rot;

	Oldpos = m_Position;
}
