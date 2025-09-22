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

	//m_mesh.Load(
	//	"assets/model/Gun/Gun_Testmodel2.fbx",				// モデル名
	//		"assets/model/Gun/");						// テクスチャのパス

	m_mesh.Load(
		"assets/model/Enemy/EnemyTes.fbx",				// モデル名
		"assets/model/Enemy/");						// テクスチャのパス


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

	m_Scale *= 3;

	//m_Rotation.x += 0.3;

	for (int i = 0; i < BulletMaxnum; i++)
	{
		e_missiles[i].Init();
	}

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	//スケール分かける
	minpos *= 3;
	maxpos *= 3;

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);

	m_interceptionSphere = std::make_unique<Sphere>(25);
}



void Enemy_Missile::Update(uint64_t deltatime)
{
	//死んでるなら移動させる
	if (HP <= 0) { m_Position = { 0.0f,-20.0f,0.0f }; return; }

	//プレイヤーから距離を取るようにする
	Vector3 P_E_Renged = m_Position - player->GetPosition();
	if (P_E_Renged.x < 0) 
	{
		P_E_Renged.x *= -1;
	}
	if (P_E_Renged.y < 0)
	{
		P_E_Renged.y *= -1;
	}
	if (P_E_Renged.z < 0)
	{
		P_E_Renged.z *= -1;
	}
	//距離判定
	if (P_E_Renged.x + P_E_Renged.y + P_E_Renged.z < 80) 
	{
		//プレイヤーから離れる
		m_Position += player->GetForward() * 0.1f;
	}
	else 
	{
		int tes = 100;
	}

	//衝突判定と無敵時間の処理
	if (collision_hit)
	{
		Invincibility_time += static_cast<float>(deltatime) / 1000;

		if (Invincibility_time > 1000)
		{
			collision_hit = false;

			Invincibility_time = 0;
		}

	}

	if (interception)
	{
		interception_time += static_cast<float>(deltatime) / 1000;

		if (interception_time > 120)
		{
			//ここオフにすると迎撃しなくなる
			//interception = false;
			interception_time = 0;
		}

	}

	

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
	

	for (int i = 0; i < BulletMaxnum; i++)
	{
		e_missiles[i].Update(deltatime);
	}

	Vector3 TargetForward = (m_Position - player->GetPosition());

	TargetForward.Normalize();

	// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
	float yaw = atan2f(TargetForward.x, TargetForward.z);
	float pitch = atan2f(-TargetForward.y,
		sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

	// 4. Roll は今回は固定 0
	m_Rotation = Vector3{ 0.0f,yaw, 0.0f };

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

void Enemy_Missile::Draw()
{
	//姿勢の補完をここでする
	/*m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;*/

	// SRT情報作成
	SRT srt;
	srt.pos = m_Position;			// 位置
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.scale = m_Scale;			// 拡縮

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	m_shader.SetGPU();

	if (HP > 0) {
		m_meshrenderer.Draw();//HPがないなら描画しない

		for (int i = 0; i < BulletMaxnum; i++)
		{
			e_missiles[i].Draw();
		}
	}

	Vector3 poscop = m_Position;//positionのコピーをとる
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	m_Position.y += 3.0f;
	m_Position += Right_vec * 3.0f;
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	m_Position = poscop;//positionは元に戻しておく
	if (collision_hit) 
	{
		m_shapecube_col->Draw(transmtx, { 0.6,0.0,0.0,0.5 });
	}
	else 
	{
		m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });
	}
	
	if (interception) 
	{
		//m_interceptionSphere->Draw(transmtx, { 0.0,0.0,0.5,0.2 });
	}
	else {
		//m_interceptionSphere->Draw(transmtx, { 1.0,1.0,1.0,0.2 });
	}

	//姿勢の補完をここでする
	/*m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;*/



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

void Enemy_Missile::Reset()
{
	HP = MaxHP;
	for (int i = 0; i < BulletMaxnum; i++)
	{
		e_missiles[i].Reset();
	}
}

GM31::GE::Collision::BoundingBoxOBB Enemy_Missile::GetOBB()
{
	Vector3 poscop = m_Position;
	Vector3 rotcop = m_Rotation;

	GM31::GE::Collision::BoundingBoxOBB obb;
	
	/*m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;*/
	m_Position.y += 1.0f;
	m_Position -= Forward_vec * 1.5f;
	
	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		m_Position,				// 中心座標（ワールド）
		Width,					// 幅
		Height,					// 高さ
		Depth);					// 奥行
	
	m_Position = poscop;
	m_Rotation = rotcop;
	return obb;
}

GM31::GE::Collision::BoundingSphere Enemy_Missile::GetShere()
{
	if (interception) return GM31::GE::Collision::BoundingSphere({ 0,-10,0 }, 1);

	return GM31::GE::Collision::BoundingSphere(m_Position, 25);
}

void Enemy_Missile::CreateBullet()
{
	if (Bulletnum == BulletMaxnum) Bulletnum = 0;

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

	e_missiles[Bulletnum].SetForward(forward);
	e_missiles[Bulletnum].SetPlayar(player);

	//前向き行列取ってから姿勢補完する
	//姿勢補完分
	/*srt.rot.x += 1.55;
	srt.rot.y += 1.55;*/
	Vector3 bulletpos = m_meshrenderer.LogBoneWorldPosition("Shot", srt);

	//指定したボーンがないならオブジェクトの中心座標から球を打つ
	if (bulletpos == Vector3::Zero) bulletpos = m_Position;

	pb->SetScale(Vector3(1, 1, 1));
	pb->SetRotation(m_Rotation);
	pb->SetPosition(bulletpos);

	e_missiles[Bulletnum].SetScale(Vector3(1, 1, 1));
	e_missiles[Bulletnum].SetRotation(m_Rotation);
	e_missiles[Bulletnum].SetPosition(bulletpos);
	e_missiles[Bulletnum].SetShot(true);
	e_missiles[Bulletnum].priod = 1000;

	//e_missile.push_back(std::move(pb));

	Bulletnum++;
}

void Enemy_Missile::SetPlayer(M_Player* pl)
{
	player = pl;
}

GM31::GE::Collision::BoundingBoxOBB Enemy_Missile::GetOBB_Bullet(int bulletnum)
{
	return e_missiles[bulletnum].GetOBB();
}

void Enemy_Missile::SetCollision(bool col)
{
	if (!col) return;

	if (!collision_hit) {
		HP--;
	}


	collision_hit = true;
}

void Enemy_Missile::SetCollision_Bullet(int num, bool col)
{
	e_missiles[num].SetCol(col);
}

