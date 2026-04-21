#include "Buller.h"
#include <random>

#include "Random.h"

Bullet::Bullet()
{
	Init();
	Reset();
}

Bullet::~Bullet()
{
}

void Bullet::Init()
{
	//属性
	Attribute = UNJOINABLE;

	//m_mesh.Load(
	//	"assets/model/Shot/laser.x",				// モデル名
	//	"assets/model/Shot/");						// テクスチャのパス

	m_mesh.Load(
		"assets/model/Gun/Bullet02.fbx",				// モデル名
		"assets/model/Gun/Tex/Tex_Blue");						// テクスチャのパス

	// 画像のUV座標
	Vector2 uv[4] = {
		Vector2(0, 0),
		Vector2(1.0f / 1.0f, 0),
		Vector2(0, 1.0f / 1.0f),
		Vector2(1.0f / 1.0f, 1.0f / 1.0f)
	};
	// マテリアル生成
	MATERIAL	mtrl_Screen;
	mtrl_Screen.Ambient = Color(0, 0, 0, 0);
	mtrl_Screen.Diffuse = Color(1, 1, 1, 1.0f);//ここが色なのでこれをいじる
	mtrl_Screen.Emission = Color(0, 0, 0, 0);
	mtrl_Screen.Specular = Color(0, 0, 0, 0);
	mtrl_Screen.Shiness = 0;
	mtrl_Screen.TextureEnable = TRUE;
	//m_Shadow = std::make_unique<CSprite>(5, 5, "assets/texture/Shadow.png", uv, mtrl_Screen);
	m_Shadow = std::make_unique<CSprite>(5, 5, "assets/texture/effect000.jpg", uv, mtrl_Screen);

	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	//m_shader.Create(
	//	"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
	//	"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);
	//原点からずらす必要があるから固定値足す
	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_Position.y = -100;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);
	Boooooooom = std::make_unique<Sphere>(5);
}
;

void Bullet::Update(uint64_t deltatime)
{
	float time_D = static_cast<float>(deltatime) / 1000;

	Alive_time -= time_D;

	if (collsion)
	{
		boom_time += time_D;

		if (boom_time > 1000)
		{
			collsion = false;

			boom_time = 0;

			m_Position = { 0,-100,0 };
		}
		return;
	}

	if (!shot) return;

	if (induction)
	{
		if (!Target_P) return;
		
		//m_Position += forward * 0.3;
		velocty;// = Vector3{ 0,0,0 };//加速値
		Vector3 Position = m_Position;//ポジション
		priod;//ピリオド
		if (priod > filstpriod/2)
		{
			priod -= time_D;

			m_Position += forward * 0.8f;
		}
		else
		{
			if (Missile) 
			{
				Missile = false;
				RandomGen rand;

				Targetpos = *Target_P;
				Targetpos.x += rand.UniformFloat(-25.0f, 25.0f);;
				Targetpos.z += rand.UniformFloat(-25.0f, 25.0f);;
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
				if (!collsion)m_Position = { 0,-200,0 };
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


			if (!collsion)m_Position = Position;
		}
	}
	else if (!collsion)m_Position += forward * 0.3f;//命中するまでは移動する

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
	
};

void Bullet::LateUpdate(uint64_t deltatime) 
{

}

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

	//m_shader.SetGPU();

	if (!collsion)m_meshrenderer.Draw();

	//弾のAABBボックス表示
	Vector3 poscop = m_Position;
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	Matrix4x4 scaleMtx = Matrix4x4::CreateScale(m_Scale); // Vector3(幅, 高さ, 奥行き)

	Matrix4x4 transmtx = m_RotationMtx  * Matrix4x4::CreateTranslation(m_Position);

	//原点とモデルの差の分ずらして再定義
	m_Position += Forward_vec * (5.0f * m_Scale);
	transmtx = m_RotationMtx * scaleMtx *Matrix4x4::CreateTranslation(m_Position);

	m_Position = poscop;//positionは元に戻しておく

	//m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.2 });
	//向き気にしなくてもいいからSRTで描画
	if (collsion) Boooooooom->Draw(transmtx, { 1.0,1.0,0.0,0.5 });
	Renderer::SetBlendState(BS_SUBTRACTION);   ///< 減算合成
	//影を落とす
	if (shot && !collsion && m_Position.y > -10)m_Shadow->Draw3D(Vector3{ 1,3,1 }, { 4.7f,m_Rotation.y,0 }, { m_Position.x,1,m_Position.z });

	// 元に戻す
	Renderer::SetBlendState(BS_ALPHABLEND);
	
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

void Bullet::Reset()
{
	m_Position = { 0,-200,0 };
	shot = false;

	//5秒持続
	Alive_time = 5000;

	//弾丸の個性を戻す
	Missile = true;
	priod = 0;
};

int Bullet::GetShaderNum()
{
	return 0;
}

GM31::GE::Collision::BoundingBoxOBB Bullet::GetOBB()
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