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

	m_Position.y = -20;

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);

	Boooooooom = std::make_unique<Sphere>(5);
}

void E_Missile::Update(uint64_t deltatime)
{

	if (collsion) 
	{
		boom_time += static_cast<float>(deltatime) / 1000;

		if (boom_time > 1000)
		{
			collsion = false;

			boom_time = 0;

			m_Position = { 0,-10,0 };
		}
	}

	if (!shot) return;

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

		velocty = { 0,0,0 };
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
		if (priod < 300)
		{
			if (priod < -1500) 
			{
				shot = false;
			}
			else 
			{
				m_Position -= Forward_vec * 2;
			}
			
		}
		else 
		{
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


		
		if (!shot) {
			velocty = { 0,0,0 };
			//何かに当たっているなら爆発させるのでポジションはそのまま
			if(!collsion)m_Position = { 0,-10,0 };
			
		}
	}

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

	if (!collsion)m_meshrenderer.Draw();

	//弾のAABBボックス表示
	Vector3 poscop = m_Position;
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;


	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	//原点とモデルの差の分ずらして再定義
	m_Position += Forward_vec * 5.0f;
	transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	m_Position = poscop;//positionは元に戻しておく

	m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.2 });

	if (collsion) Boooooooom->Draw(transmtx, { 1.0,1.0,0.0,0.5 });
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

void E_Missile::Reset()
{
	m_Position = { 0,-20,0 };
	shot = false;
}

GM31::GE::Collision::BoundingBoxOBB E_Missile::GetOBB()
{
	GM31::GE::Collision::BoundingBoxOBB obb;

	if (collsion) return obb;

	//forwardベクトルはあえて精度の悪いものを使っているため補正はちゃんとした取り方する
	Vector3 poscop = m_Position;
	//原点とモデルの差の分ずらして再定義
	m_Position += Forward_vec * 5.0f;

	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		m_Position,				// 中心座標（ワールド）
		Width,					// 幅
		Height,					// 高さ
		Depth);					// 奥行

	m_Position = poscop;

	return obb;
}
