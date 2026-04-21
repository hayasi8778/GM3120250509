#include "Player_LeftArm.h"

void Player_LeftArm::Init(Vector3* rot)
{
	MainRotation = rot;
	Init();
}

void Player_LeftArm::Init()
{
	//属性
	Attribute = PLAYER;
	MyType = CHARACTER;
	//プレイヤーなので接触フラグは最初からon
	adhesioing = true;

	//ロボットモデル(左腕)
	//m_mesh.Load(
	//	"assets/model/Mec/MecBone_LeftArm.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス
	
	//バイナリ込みの読み込み
	//m_mesh.Load(
	//	"assets/model/Mec/MecBone_LeftArm_TestModel.fbx",				// モデル名
	//	"assets/model/Mec/");						// テクスチャのパス

	//
	m_mesh.LoadToAssimp(
		"assets/model/Mec/MecBone_LeftArm_TestModel.fbx",				// モデル名
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
	m_Position.y += 9.0f;

	m_Rotation.z -= 1.40f;

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);
	m_Rotation.z += 1.40f;
}

void Player_LeftArm::Update(uint64_t deltatime)
{
	if (recoil != 0) 
	{
		recoil -= 0.2f;//銃を撃った反動をそれっぽくする
		if (recoil < 0) recoil = 0.0f;
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

	if (armfloat) 
	{
		Armrot += 0.005f;
		if (Armrot > -1.3f) 
		{
			armfloat = false;
		}
	}
	else 
	{
		Armrot -= 0.005f;
		if (Armrot < -1.6f)
		{
			armfloat = true;
		}
	}
}

void Player_LeftArm::Dispose()
{

}

void Player_LeftArm::LateUpdate(uint64_t deltatime) 
{
	//座標が決定した後に追従させる
	if (Connectableobject != nullptr) {
		//接続しているオブジェクトを追従させる
		Connectableobject->SetPosition(Conectpos("Hand"));//場所
		Vector3 coprot = m_Rotation;
		coprot.y -= 1.4f;
		Connectableobject->SetRotation(coprot);//角度
	}
}

void Player_LeftArm::Draw()
{
	//姿勢の補完をここでする
	/*m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;*/

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	srt.pos += Right_vec * 1.5f;
	//srt.rot.z += Armrot;

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

	m_Position += Right_vec * 4.0f;
	m_Position += Forward_vec * 0.5f;
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);
	
	m_Position = poscop;

	//if (col) {
	//	m_shapecube_col->Draw(transmtx, { 0.6,0.0,0.0,0.5 });
	//}
	//else
	//{
	//	m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });
	//}


	/*m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;*/
}

void Player_LeftArm::Adhesioing()
{

}

void Player_LeftArm::Action(Vector3 vec)
{
	recoil = 3.0f;
	if (Connectableobject)Connectableobject->Action(vec);
}

void Player_LeftArm::Reset()
{

}

int Player_LeftArm::GetShaderNum()
{
	return 0;
}

GM31::GE::Collision::BoundingBoxOBB Player_LeftArm::GetOBB()
{
	GM31::GE::Collision::BoundingBoxOBB obb;

	//姿勢の補完をここでする
	/*m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;*/

	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		m_Position,				// 中心座標（ワールド）
		Width,					// 幅
		Height,					// 高さ
		Depth);					// 奥行

	//姿勢の補完をここでする
	/*m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;*/

	return obb;
}

Vector3 Player_LeftArm::Conectpos(const std::string& targetName)
{
	Vector3 conect = { 0.0f,0.0f,0.0f };
	SRT srt;

	srt.scale = m_Scale;
	srt.rot = m_Rotation;
	srt.pos = m_Position;

	conect = m_meshrenderer.LogBoneWorldPosition(targetName, srt);
	
	//conect = m_meshrenderer.LogBoneWorldPosition(3, srt);

	return conect;
}

void Player_LeftArm::Rockon(Vector3 rot)
{
	// 1. forward ベクトルを計算
	Vector3 Forward = -(rot - m_Position);

	Forward.Normalize();

	// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
	float yaw = atan2f(Forward.x, Forward.z);
	float pitch = atan2f(-Forward.y,
		sqrtf(Forward.x * Forward.x + Forward.z * Forward.z));

	// 1) オイラー → Quaternion
	Quaternion qBase = Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f);

	// 2) Up 軸まわりに 90° 回転する Quaternion
	Quaternion q90 = Quaternion::CreateFromAxisAngle(Up_vec, 3.141592f / 2);

	// 3) 合成
	Quaternion qFinal = qBase * q90;

	// 4. Roll は今回は固定 0
	m_Rotation = Vector3{ 0.0f, yaw + 1.4f,pitch +recoil };//90度曲げるから例外的にRall yaw pitchの順に入れる
	
}

void Player_LeftArm::Conect(Object* obj)
{
	Connectableobject = obj;
	//接続フラグをonにして接続時の処理を通す
	obj->SetAdhesioing(true);
	obj->Adhesioing();
}

void Player_LeftArm::Release()
{
	if (Connectableobject) Connectableobject->SetAdhesioing(false);
	Connectableobject = nullptr;
}
