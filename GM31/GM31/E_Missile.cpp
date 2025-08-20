#include "E_Missile.h"

E_Missile::E_Missile()
{
	Init();
}

E_Missile::~E_Missile()
{
}

void E_Missile::Init()
{
	//属性
	Attribute = UNJOINABLE;

	m_mesh.Load(
		"assets/model/starwars/laser.x",		// モデル名
		"assets/model/starwars/");				// テクスチャのパス;

	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	m_shader.Create(
		"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);
}

void E_Missile::Update(uint64_t deltatime)
{
	float time_D = static_cast<float>(deltatime) / 1000;
	//m_Position += forward * 0.3;
	velocty;// = Vector3{ 0,0,0 };//加速値
	Vector3 Position = m_Position;//ポジション
	player;//ターゲット
	priod;//ピリオド
	if (priod > 500) 
	{
		priod -= time_D;

		m_Position += forward * 0.4f;
	}
	else 
	{
		Vector3 acceleration = Vector3{ 0,0,0 };
		//帰り
		Vector3 diff = player->GetPosition() - m_Position;

		acceleration += (diff - velocty * priod) * 2 / (priod * priod);

		if (acceleration.Length() > 0.03f)
		{
			acceleration.Normalize(acceleration);

			acceleration *= 0.03f;
		}

		priod -= time_D;
		if (priod < 0)
		{
			velocty = { 0,0,0 };
		}

		velocty += acceleration * time_D;
		Position += velocty * time_D;
		/*velocty += acceleration * Time;
		Position += velocty * Time;*/

		//座標更新前に角度を更新する
		// 事前に #include <cmath> などが必要
		const float eps = 1e-6f;

		// 1. forward ベクトルを計算
		Vector3 Forward = -(Position - m_Position);

		// 2. 動きがある場合のみ回転計算
		if (Forward.LengthSquared() > eps) {
			Forward.Normalize();

			// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
			float yaw = atan2f(Forward.x, Forward.z);
			float pitch = atan2f(-Forward.y,
				sqrtf(Forward.x * Forward.x + Forward.z * Forward.z));

			// 4. Roll は今回は固定 0
			m_Rotation = Vector3{ pitch, yaw, 0.0f };
		}


		m_Position = Position;
	}
}

void E_Missile::Draw()
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
}

void E_Missile::Dispose()
{

}

void E_Missile::Adhesioing()
{

}

void E_Missile::Action(Vector3 vec)
{

}

GM31::GE::Collision::BoundingBoxOBB E_Missile::GetOBB()
{
	GM31::GE::Collision::BoundingBoxOBB obb;

	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		m_Position,				// 中心座標（ワールド）
		Width,					// 幅
		Height,					// 高さ
		Depth);					// 奥行

	return obb;
}
