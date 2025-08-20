#include "Buller.h"
#include <random>

Bullet::Bullet()
{
	Init();
}

Bullet::~Bullet()
{
}

void Bullet::Init()
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
	//原点からずらす必要があるから固定値足す
	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);
}
;

void Bullet::Update(uint64_t deltatime)
{

	if (!shot) return;

	if (induction)
	{
		if (!Target_P) return;
		float time_D = static_cast<float>(deltatime) / 1000;
		//m_Position += forward * 0.3;
		velocty;// = Vector3{ 0,0,0 };//加速値
		Vector3 Position = m_Position;//ポジション
		priod;//ピリオド
		if (priod > filstpriod/2)
		{
			priod -= time_D;

			m_Position += forward * 0.4f;
		}
		else
		{
			if (Missile) 
			{
				Missile = false;
				std::mt19937 rad{ std::random_device{}() };
				std::uniform_real_distribution<float> posdist{ -10.0f, 10.0f };

				Targetpos = *Target_P;
				Targetpos.x += posdist(rad);
				Targetpos.z += posdist(rad);
			}
			Vector3 acceleration = Vector3{ 0,0,0 };
			//帰り
			/*Vector3 diff = Targetpos - m_Position;*/
			Vector3 diff = Targetpos - m_Position;

			acceleration += (diff - velocty * priod) * 2 / (priod * priod);

			if (acceleration.Length() > 0.03f)
			{
				acceleration.Normalize(acceleration);

				acceleration *= 0.03f;
			}

			priod -= time_D;
			if (priod < 0)
			{
				shot = true;
				m_Position = { 0,0,0 };
				Missile = true;
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
	else m_Position += forward * 0.3;
	
};

void Bullet::Draw()
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

	//弾のAABBボックス表示
	Vector3 poscop = m_Position;
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;


	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	// forward 抽出
	Vector3 forward(transmtx._31, transmtx._32, transmtx._33);
	forward.Normalize();

	//原点とモデルの差の分ずらして再定義
	m_Position += forward * 5.0f;
	transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	m_Position = poscop;//positionは元に戻しておく

	m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.2 });
};

void Bullet::Dispose()
{

};

void Bullet::Adhesioing()
{

};

void Bullet::Action(Vector3 vec)
{

}

GM31::GE::Collision::BoundingBoxOBB Bullet::GetOBB()
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

void Bullet::Setinduction(float pr ,Vector3 forwa)
{
	induction = true;

	priod = pr;

	filstpriod = priod;

	forward = forwa;

	shot = true;

	//速度を初期化
	velocty = Vector3{ 0,0,0 };
}
;