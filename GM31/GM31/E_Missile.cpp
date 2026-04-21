#include "E_Missile.h"

Vector3 RotateAroundAxis(const Vector3&, const Vector3&, float);

E_Missile::E_Missile()
{
	Init();
	Reset();
}

E_Missile::~E_Missile()
{
}

void E_Missile::Init()
{
	//属性
	Attribute = UNJOINABLE;

	m_mesh.Load(
		"assets/model/Gun/Bullet002.fbx",				// モデル名
		"assets/model/Gun/Tex/Tex_Red");						// テクスチャのパス


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
	mtrl_Screen.Diffuse = Color(1, 1, 1, 0.3f);//ここが色なのでこれをいじる
	mtrl_Screen.Emission = Color(0, 0, 0, 0);
	mtrl_Screen.Specular = Color(0, 0, 0, 0);
	mtrl_Screen.Shiness = 0;
	mtrl_Screen.TextureEnable = TRUE;
	//m_Shadow = std::make_unique<CSprite>(5, 5, "assets/texture/Shadow.png", uv , mtrl_Screen);
	m_Shadow = std::make_unique<CSprite>(5, 5, "assets/texture/effect000.jpg", uv, mtrl_Screen);

	/*m_BlobShadow = std::make_unique<BlobShadow>();
	m_BlobShadow->Init("assets/texture/effect000.jpg");*/

	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	//m_shader.Create(
	//	"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
	//	"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー

	m_Position.y = -20;

	/*m_Scale.x = 1.5f;
	m_Scale.y = 1.5f;
	m_Scale.z = 1.5f;*/

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);

	Boooooooom = std::make_unique<Sphere>(5);



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

void E_Missile::Update(uint64_t deltatime)
{

	//Update_Shot1(deltatime);
	Update_Shot2(deltatime);

	//3軸ベクトルの取り直し
	ResetVector();
}

void E_Missile::LateUpdate(uint64_t deltatime) 
{

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


	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	//原点とモデルの差の分ずらして再定義
	//m_Position += Forward_vec * 5.0f;
	transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	m_Position = poscop;//positionは元に戻しておく

	m_shapecube_col->Draw(transmtx, { 1.0f,1.0f,1.0f,0.2f });

	if (collsion) Boooooooom->Draw(transmtx, { 1.0f,1.0f,0.0f,0.5f });


	Renderer::SetBlendState(BS_SUBTRACTION);   ///< 減算合成
	//影を落とす
	if (shot)m_Shadow->Draw3D(Vector3{ 1,3,1 }, { 4.7f,m_Rotation.y,0 }, { m_Position.x,1,m_Position.z });
	//if (shot) m_BlobShadow->Draw(5.0f, 0.1f, m_Position.x, m_Position.z);

	// 元に戻す
	Renderer::SetBlendState(BS_ALPHABLEND);
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
	count = 3; //最初の数フレームは角度上限付けずに追う
	
	Alive_time = 5000;	//弾丸の生存時間

	//弾丸の個性を戻す
	maxTurn = DefaultMaxTurn; //最大向き角度を元に戻す
	Turn = DefaultMaxTurn; //最大向き角度を元に戻す
	ShotSpeed = DefaultShotSpeed;
}

int E_Missile::GetShaderNum()
{
	return 0;
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

void E_Missile::ResetVector()
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

void E_Missile::SetObject(Object* pl) {
	player = pl;

	//プレイヤーの方へ向ける
	Vector3 TargetForward = (player->GetPosition() - m_Position);
	TargetForward.y = 0;
	TargetForward.Normalize();

	// Yaw: Y軸回り（XZ平面）
	float yaw = atan2f(TargetForward.x, TargetForward.z);

	// Pitch: X軸回り（上下角度）
	float pitch = atan2f(-TargetForward.y,
		sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

	// Roll は不要なら 0 固定
	m_Rotation = Vector3{ pitch, yaw, 0.0f };




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

void E_Missile::Update_Shot1(uint64_t deltatime)
{
	if (collsion) //弾がぶつかったら爆発する
	{
		boom_time += static_cast<float>(deltatime) / 1000;

		if (boom_time > 1000)
		{
			collsion = false;

			boom_time = 0;

			m_Position = { 0,-10,0 };
		}
		return;
	}

	//射撃されたフラグが経ってないなら更新しない
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
			if (!collsion)m_Position = { 0,-10,0 };

		}
	}
}

void E_Missile::Update_Shot2(uint64_t deltatime)
{
	float time_D = static_cast<float>(deltatime) / 1000;

	if (collsion) //弾がぶつかったら爆発する
	{
		boom_time += time_D;

		if (boom_time > 1000)
		{
			collsion = false;

			boom_time = 0;

			m_Position = { 0,-10,0 };
		}
		return;
	}

	//射撃されたフラグが経ってないなら更新しない
	if (!shot) return;

	Alive_time -= time_D;
	//時間切れで弾消す
	if (Alive_time < 0) {
		//shot = false;
		collsion = true;
		shot = false;
	}

	//地面にぶつかっても爆発する
	if (m_Position.y < GroundHight) {
		collsion = true;
		shot = false;
	}

	//通常と違う追尾性持っているならここを通る
	if (maxTurn != DefaultMaxTurn) 
	{
		Turn_Time -= time_D;
		if (Turn_Time < 0) {
			Turn_Time = 0;
			maxTurn = Turn;
		}
	}
	
	//デバック用コード
	if (false) {
		// 移動
		m_Position += Forward_vec * ShotSpeed * time_D;
		return;
	}


	// ターゲット方向
	Vector3 PLpos = player->GetPosition();
	if (int(Turn_Time) % 1000 > 500 && m_Position.y - PLpos.y < 10.0f) { 
		PLpos.y += 10; }
	Vector3 toTarget = PLpos - m_Position;
	if (toTarget.LengthSquared() > 1e-6f) {
		toTarget.Normalize();

		// 現在の進行方向との差
		float dotVal = Dot(Forward_vec, toTarget);
		dotVal = std::clamp(dotVal, -1.0f, 1.0f);
		float angleDiff = acosf(dotVal);

		if (angleDiff > maxTurn) {
			/*
			float t = 1;
			if (count > 0) {//最初の数フレームは完全に追従させる
				count--;
			}
			else { t = maxTurn / angleDiff;
			t = std::clamp(t, 0.0f, 1.0f);//tが１を上回るとバグるのでここで補完する
			}
			Forward_vec = (Vector3::Lerp(Forward_vec, toTarget, t));
			*/

			float turnAngle = 0.0f;

			if (count > 0) {
				// 発射直後は完全追従
				count--;
				turnAngle = angleDiff;   // つまり toTarget へ一気に向ける
			}
			else {
				// 最大旋回角で追尾
				turnAngle = maxTurn;
			}

			// 回転軸
			Vector3 axis = Forward_vec.Cross(toTarget);
			if (axis.LengthSquared() < 1e-6f) {
				axis = Vector3(0, 1, 0); // ほぼ同方向 or 真逆方向の保険
			}
			axis.Normalize();

			// Forward_vec を axis まわりに turnAngle 回転
			Forward_vec = RotateAroundAxis(Forward_vec, axis, turnAngle);
			Forward_vec.Normalize();


		}
		else {
			Forward_vec = toTarget;
		}

		//向き調節終わった後に傾ける
		Vector3 Forward = Forward_vec;
		Forward.Normalize();

		// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
		float yaw = atan2f(Forward.x, Forward.z);
		float pitch = atan2f(-Forward.y,
			sqrtf(Forward.x * Forward.x + Forward.z * Forward.z));

		// 4. Roll は今回は固定 0
		m_Rotation = Vector3{ pitch, yaw, 0.0f };
	}

	// 移動
	m_Position += Forward_vec * ShotSpeed * time_D;

}

Vector3 RotateAroundAxis(const Vector3& v, const Vector3& axis, float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);

	// axis.Cross(v) に修正
	return v * c + axis.Cross(v) * s + axis * axis.Dot(v) * (1 - c);
}
