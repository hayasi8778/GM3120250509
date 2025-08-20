#include "M_Rock.h"



void M_Rock::Init()
{
	//属性
	Attribute = JOINABLE;

	// モデルの初期化
	m_mesh.Load(
		"assets/model/car000.x",				// モデル名
		"assets/model/");						// テクスチャのパス
	

	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	m_shader.Create(
		"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	//	DebugUI::RedistDebugFunction(DebugPlayerMoveParameter);

	//m_Position.x = 1000;

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);

}

void M_Rock::Update(uint64_t deltatime)
{

	if (shootflag) 
	{
		float time_D = static_cast<float>(deltatime) / 1000;
		if (returnRock) 
		{
			velocty;// = Vector3{ 0,0,0 };//加速値
			Vector3 Position = m_Position;//ポジション
			player;//ターゲット
			priod;//ピリオド
			Vector3 acceleration = Vector3{ 0,0,0 };
			//帰り
			Vector3 diff = player->GetPosition() - m_Position;

			acceleration += (diff - velocty * priod) * 2 / (priod * priod);
			if (acceleration.Length() > 0.03f)
			{
				int tes = 100;
			}
			

			//m_Position -= forward * 0.1f;雑な式
			priod-= time_D;
			/*priod -= Time;*/
			if (priod < 0)
			{
				shootflag = false;
				adhesioing = true;
				velocty = { 0,0,0 };
				Attribute = JOINABLE;
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
		else 
		{
			//行き
			m_Position += forward * 1.0f;
			life-= time_D;
			if (life < 0)
			{
				if (player) 
				{
					//m_Rotation.y += 3;
					velocty = Vector3{ 0,0,0 };
				}
				else 
				{
					m_Rotation.y += 3;
				}
				
				returnRock = true;
			}
		}
	}
	else if (!adhesioing) //重力
	{
		m_Position.y -= 0.4;
	}
	if (m_Position.y < 0) //地面にめり込まないようにする
	{
		m_Position.y = 0;
	}
}

void M_Rock::Draw()
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

	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	m_Position.y += 4;
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);
	m_Position.y -= 4;

	m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });

	//// デバッグ用のグローバル変数に値をセット
	//g_position = m_Position;
	//g_rotation = m_Rotation;
	//g_scale = m_Scale;
}

void M_Rock::Dispose()
{

}

void M_Rock::Adhesioing()
{

}

void M_Rock::Action(Vector3 vec)
{
	//岩は前に射出して戻ってこさせる
	//adhesioing = false;
	Attribute = UNJOINABLE;
	shootflag = true;
	returnRock = false;
	life = DefaultLife;
	priod = DefaultLife;
	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	Matrix4x4 world = srt.GetMatrix();
	forward = world.Forward();
	forward.Normalize();
	forward *= 3.0f;
}

GM31::GE::Collision::BoundingBoxOBB M_Rock::GetOBB()
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

void M_Rock::SetPlayer(M_Player* pl)
{
	player = pl;
}
