#include "E_Beam02.h"

E_Beam02::E_Beam02()
{
}

E_Beam02::~E_Beam02()
{
}

void E_Beam02::Init()
{
	//属性
	Attribute = UNJOINABLE;

	m_mesh.Load(
		"assets/model/Shot/Beam02.fbx",		// モデル名
		"assets/model/Shot/");				// テクスチャのパス;

	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	//m_shader.Create(
	//	"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
	//	"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー

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

void E_Beam02::Update(uint64_t deltatime)
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

}

void E_Beam02::LateUpdate(uint64_t deltatime)
{

}

void E_Beam02::Draw()
{
	// SRT情報作成
	SRT srt;
	srt.pos = m_Position;			// 位置
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.scale = m_Scale * 1.5f;			// 拡縮

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	//m_shader.SetGPU();

	if (!collsion)m_meshrenderer.Draw();

	//弾のAABBボックス表示
	Vector3 poscop = m_Position;
	//スケール行列を作成

	Matrix4x4 scaleMtx = Matrix4x4::CreateScale(srt.scale);
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	//原点とモデルの差の分ずらして再定義
	m_Position -= Forward_vec * (m_Scale.z * 1.5f);
	//m_Position -= Forward_vec;
	Matrix4x4 transmtx = scaleMtx * m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);


	m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.2 });

	if (collsion) Boooooooom->Draw(transmtx, { 1.0,1.0,0.0,0.5 });

	m_Position = poscop;//positionは元に戻しておく
}

void E_Beam02::Dispose()
{

}

void E_Beam02::Adhesioing()
{

}

void E_Beam02::Action(Vector3 vec)
{

}

void E_Beam02::Reset()
{
	m_Position = { 0,-20,0 };
}

int E_Beam02::GetShaderNum()
{
	return 0;
}

GM31::GE::Collision::BoundingBoxOBB E_Beam02::GetOBB()
{
	GM31::GE::Collision::BoundingBoxOBB obb;

	if (collsion) return obb;
	Vector3 poscop = m_Position;
	//原点とモデルの差の分ずらして再定義
	//m_Position += Forward_vec * 5.0f;
	m_Position -= Forward_vec * (m_Scale.z * 1.5f);

	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		m_Position,				// 中心座標（ワールド）
		Width * m_Scale.x,					// 幅
		Height * m_Scale.y,					// 高さ
		Depth * m_Scale.z);					// 奥行

	m_Position = poscop;

	return obb;
}