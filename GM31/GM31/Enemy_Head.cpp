#include "Enemy_Head.h"

void Enemy_Head::Init()
{
	//属性
	Attribute = PLAYER;
	MyType = CHARACTER;
	//プレイヤーなので接触フラグは最初からon
	adhesioing = true;

	////ロボットモデル(頭)
	//m_mesh.Load(
	//	"assets/model/Mec/MecBone_Head.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	m_mesh.Load(
		"assets/model/Mec/MecBone_Head_Base.fbx",				// モデル名
		"assets/model/Mec/MecArm_cop/Base_Green");						// テクスチャのパス

	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	m_Rotation = m_meshrenderer.GetModelRot();

	// シェーダーの初期化
	//m_shader.Create(
	//	"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
	//	"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	//	DebugUI::RedistDebugFunction(DebugPlayerMoveParameter);

	//スケール調整
	SetScale({ 1.0f,1.0f,1.0f });

	//位置補正
	m_Position.y += 9;

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);
}

void Enemy_Head::Update(uint64_t deltatime)
{
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

void Enemy_Head::LateUpdate(uint64_t deltatime)
{
}

void Enemy_Head::Dispose()
{

}

void Enemy_Head::Draw()
{
	//姿勢の補完をここでする
	m_Rotation.x += 1.55f;
	m_Rotation.y += 1.55f;

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	//m_shader.SetGPU();

	m_meshrenderer.Draw();

	//これとOBBはシーンシェーダー上書きするからdebug時のみ使う
	//m_meshrenderer.DrawWithBones(srt, { 1.0f, 1.0f, 0.0f });


	//// デバッグ用のグローバル変数に値をセット
	//g_position = m_Position;
	//g_rotation = m_Rotation;
	//g_scale = m_Scale;

	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	m_Position += Up_vec * 1;
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);
	m_Position -= Up_vec * 1;

	if (col) {
		m_shapecube_col->Draw(transmtx, { 0.6,0.0,0.0,0.5 });
	}
	else
	{
		//m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });
	}


	m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;
}

void Enemy_Head::Adhesioing()
{

}

void Enemy_Head::Action(Vector3 vec)
{

}

void Enemy_Head::Reset()
{
	m_Scale = { 1,1,1 };
}

int Enemy_Head::GetShaderNum()
{
	return 0;
}

GM31::GE::Collision::BoundingBoxOBB Enemy_Head::GetOBB()
{
	GM31::GE::Collision::BoundingBoxOBB obb;

	//姿勢の補完をここでする
	m_Rotation.x += 1.55f;
	m_Rotation.y += 1.55f;

	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		m_Position,				// 中心座標（ワールド）
		Width,					// 幅
		Height,					// 高さ
		Depth);					// 奥行

	//姿勢の補完をここでする
	m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;

	return obb;
}

Vector3 Enemy_Head::Conectpos(const std::string& targetName)
{
	Vector3 conect = { 0,0,0 };

	Vector3 rotcop = m_Rotation;

	//姿勢補完分
	rotcop.x += 1.55f;
	rotcop.y += 1.55f;
	SRT srt;

	srt.scale = m_Scale;
	srt.rot = rotcop;
	srt.pos = m_Position;

	conect = m_meshrenderer.LogBoneWorldPosition(targetName, srt);

	return conect;
}

void Enemy_Head::SetMoveState(int state) 
{
	//最悪2重チェックになってもいいのでswitch入る前にデバック状態かチェックする
#ifdef _DEBUG
	switch (state) {
	case 0:
		m_mesh.Load(
			"assets/model/Mec/MecBone_Head_Base.fbx",				// モデル名
			"assets/model/Mec/MecArm_cop/Base_Green");						// テクスチャのパス
		m_meshrenderer.Init(m_mesh);

		break;
	case 1:
		m_mesh.Load(
			"assets/model/Mec/MecBone_Head_Base.fbx",				// モデル名
			"assets/model/Mec/MecArm_cop/Base_Red");						// テクスチャのパス
		m_meshrenderer.Init(m_mesh);

		break;
	case 2:
		m_mesh.Load(
			"assets/model/Mec/MecBone_Head_Base.fbx",				// モデル名
			"assets/model/Mec/MecArm_cop/Base_White");						// テクスチャのパス
		m_meshrenderer.Init(m_mesh);

		break;
	case 3:
		m_mesh.Load(
			"assets/model/Mec/MecBone_Head_Base.fbx",				// モデル名
			"assets/model/Mec/MecArm_cop/Base_Black");						// テクスチャのパス
		m_meshrenderer.Init(m_mesh);
		break;
	case 4:
		break;
	case 5:
		break;
	default:
		break;
	}
#endif
}
