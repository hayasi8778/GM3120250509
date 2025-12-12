#include "Player_RightFeet.h"

void Player_RightFeet::Init(Vector3* rot)
{
	MainRotation = rot;
	Init();
}

void Player_RightFeet::Init()
{
	//属性
	Attribute = PLAYER;
	MyType = CHARACTER;
	//プレイヤーなので接触フラグは最初からon
	adhesioing = true;

	//ロボットモデル(頭)
	m_mesh.Load(
		"assets/model/Mec/MecBone_RightFeet.fbx",				// モデル名
		"assets/model/Mec/");						// テクスチャのパス

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

void Player_RightFeet::Update(uint64_t deltatime)
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

void Player_RightFeet::LateUpdate(uint64_t deltatime) 
{

}

void Player_RightFeet::Dispose()
{

}

void Player_RightFeet::Draw()
{
	//姿勢の補完をここでする
	m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;

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

	Vector3 poscop = m_Position;

	m_Position -= Up_vec * 3.5;
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);
	m_Position = poscop;

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

void Player_RightFeet::Adhesioing()
{

}

void Player_RightFeet::Action(Vector3 vec)
{
	if (Connectableobject)Connectableobject->Action(Vector3(0.0f, 0.0f, 0.0f));
}

void Player_RightFeet::Reset()
{

}

int Player_RightFeet::GetShaderNum()
{
	return 0;
}

GM31::GE::Collision::BoundingBoxOBB Player_RightFeet::GetOBB()
{
	GM31::GE::Collision::BoundingBoxOBB obb;

	//姿勢の補完をここでする
	m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;

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

Vector3 Player_RightFeet::Conectpos(const std::string& targetName)
{
	Vector3 conect = { 0,0,0 };

	Vector3 rotcop = m_Rotation;

	//姿勢補完分
	rotcop.x += 1.55;
	rotcop.y += 1.55;
	SRT srt;

	srt.scale = m_Scale;
	srt.rot = rotcop;
	srt.pos = m_Position;

	conect = m_meshrenderer.LogBoneWorldPosition(targetName, srt);

	return conect;
}

void Player_RightFeet::Conect(Object* obj)
{
	Connectableobject = obj;
	//接続フラグをonにして接続時の処理を通す
	obj->SetAdhesioing(true);
	obj->Adhesioing();
}

void Player_RightFeet::Release()
{
	if (Connectableobject) Connectableobject->SetAdhesioing(false);
	Connectableobject = nullptr;
}