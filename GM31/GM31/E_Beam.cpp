#include"E_Beam.h"

E_Beam::E_Beam()
{
	Init();
}

E_Beam::~E_Beam()
{
}

void E_Beam::Init()
{
	//属性
	Attribute = UNJOINABLE;

	m_mesh.Load(
		"assets/model/Shot/Beam01.fbx",		// モデル名
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

	//ビームの本体
	beam02.Init();
}

void E_Beam::Update(uint64_t deltatime)
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
	
	//ビームの本体
	
	beam02.Update(deltatime);
	float scalecopz = m_Scale.z;
	if (scalecopz > 15.0f) scalecopz = 15;
	beam02.SetPosition(m_Position - (Forward_vec * (3.5f * scalecopz)));//元のオブジェクトサイズ＋サイズ分ずらす
	beam02.SetScale(m_Scale);
	beam02.SetRotation(m_Rotation);
	beam02.SetRightVec(Right_vec);
	beam02.SetUpVec(Up_vec);
	beam02.SetForwardVec(Forward_vec);

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

void E_Beam::Draw()
{
	//ビームの前半部分はZスケール無視する
	Vector3 scalecop = m_Scale;
	if (scalecop.x > 15.0f) scalecop.x = 15;
	if (scalecop.y > 15.0f) scalecop.y = 15;
	if (scalecop.z > 15.0f) scalecop.z = 15;
	// SRT情報作成
	SRT srt;
	srt.pos = m_Position;			// 位置
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.scale = scalecop;			// 拡縮

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	//m_shader.SetGPU();

	if (!collsion)m_meshrenderer.Draw();

	//弾のAABBボックス表示
	Vector3 poscop = m_Position;
	//スケール行列を作成
	
	Matrix4x4 scaleMtx = Matrix4x4::CreateScale(scalecop);
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	//原点とモデルの差の分ずらして再定義
	//m_Position -= Forward_vec * (5.0f + (m_Scale.z *2));																																																																																																																																					
	//m_Position -= Forward_vec * (5.0f);
	Matrix4x4 transmtx = scaleMtx * m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);


	m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.2 });

	if (collsion) Boooooooom->Draw(transmtx, { 1.0,1.0,0.0,0.5 });
	m_Position = poscop;//positionは元に戻しておく

	beam02.Draw();
}

void E_Beam::LateUpdate(uint64_t deltatime) 
{

}

void E_Beam::Dispose()
{

}

void E_Beam::Adhesioing()
{

}

void E_Beam::Action(Vector3 vec)
{

}

void E_Beam::Reset()
{
	m_Position = { 0,-20,0 };
	shot = false;
}

int E_Beam::GetShaderNum()
{
	return 0;
}

GM31::GE::Collision::BoundingBoxOBB E_Beam::GetOBB()
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
		Width * m_Scale.x,					// 幅
		Height * m_Scale.y,					// 高さ
		Depth * m_Scale.z);					// 奥行

	m_Position = poscop;

	return obb;
}

GM31::GE::Collision::BoundingBoxOBB E_Beam::GetOBB02()
{
	return beam02.GetOBB();
}
