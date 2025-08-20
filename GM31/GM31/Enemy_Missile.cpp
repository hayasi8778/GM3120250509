#include "Enemy_Missile.h"

Enemy_Missile::Enemy_Missile()
{
}

Enemy_Missile::~Enemy_Missile()
{
}

void Enemy_Missile::Init()
{
	//属性
	Attribute = ENEMY;

	m_mesh.Load(
		"assets/model/Mec/Gun2.fbx",				// モデル名
		"assets/model/Mec/");						// テクスチャのパス


	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	m_shader.Create(
		"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	m_Position.z += 50;
	m_Position.y += 10;

	//m_Rotation.x += 0.3;

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);
}



void Enemy_Missile::Update(uint64_t deltatime)
{
	//m_Position.z += 0.1;

	//m_Rotation.y += 0.1;
	//m_Rotation.z += 0.1;
	//m_Rotation.x += 0.1;

	//銃弾の発生
	float time_D = static_cast<float>(deltatime) / 1000;
	
	/*priod -= Time;*/
	if (FIRE)
	{
		cooltime -= time_D;
		if (cooltime < 0)
		{
			cooltime = 1000;

			CreateBullet();
		}
	}
	
	// 移動(弾丸)
	for (auto& pb : e_missile) {

		pb->Update(deltatime);
		pb->Life--;
		if (pb->Life <= 0) {
			pb->erase = true;
		}
	}

	// 削除フラグがTRUEになっているものを消す
	std::erase_if(e_missile, [](const std::unique_ptr<E_Missile>& b) {

		return b->erase;
		});

	//弾のAABBボックス表示
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;


	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	// forward 抽出
	forward = { transmtx._31, transmtx._32, transmtx._33 };
	forward.Normalize();
	
}

void Enemy_Missile::Draw()
{
	//姿勢の補完をここでする
	m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;

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

	// 移動
	for (auto& pb : e_missile) {

		pb->Draw();
	}


	Vector3 poscop = m_Position;//positionのコピーをとる
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	m_Position.y += 1.0f;
	m_Position -= forward * 2.0f;
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	m_Position = poscop;//positionは元に戻しておく
	m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });

	//姿勢の補完をここでする
	m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;

}

void Enemy_Missile::Dispose()
{

}

void Enemy_Missile::Adhesioing()
{
	
}

void Enemy_Missile::Action(Vector3 vec)
{
	//bool型の反転
	if (FIRE) 
	{
		FIRE = false;
	}
	else 
	{
		FIRE = true;
	}
}

GM31::GE::Collision::BoundingBoxOBB Enemy_Missile::GetOBB()
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

void Enemy_Missile::CreateBullet()
{
	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	//新しい弾を作る
	std::unique_ptr<E_Missile> pb = std::make_unique<E_Missile>();

	Matrix4x4 world = srt.GetMatrix();
	Vector3 forward = world.Forward();
	forward.Normalize();
	forward *= 3.0f;

	pb->SetForward(forward);
	pb->SetPlayar(player);

	//前向き行列取ってから姿勢補完する
	//姿勢補完分
	srt.rot.x += 1.55;
	srt.rot.y += 1.55;
	Vector3 bulletpos = m_meshrenderer.LogBoneWorldPosition("Shot", srt);

	pb->SetScale(Vector3(1, 1, 1));
	pb->SetRotation(m_Rotation);
	pb->SetPosition(bulletpos);

	e_missile.push_back(std::move(pb));
}

void Enemy_Missile::SetPlayer(M_Player* pl)
{
	player = pl;
}

