#include "Enemy_Missile.h"

#include "Random.h"

#include <DirectXMath.h>

#include "system/SphereDrawer.h"
#include "system/ConeDrawer.h"

using namespace DirectX;

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
	MyType = CHARACTER;
	//m_mesh.Load(
	//	"assets/model/Gun/Gun_Testmodel2.fbx",				// モデル名
	//		"assets/model/Gun/");						// テクスチャのパス

	//m_mesh.Load(
	//	"assets/model/Enemy/EnemyTes.fbx",				// モデル名
	//	"assets/model/Enemy/");						// テクスチャのパス

	m_mesh.Load(
		"assets/model/Mec/MecBone_Body.fbx",				// モデル名
		"assets/model/Mec/");						// テクスチャのパス


	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	//m_shader.Create(
	//	"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
	//	"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	m_Position.z += 50;
	m_Position.y += 10;

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

	for (int i = 0; i < BulletMaxnum; i++)
	{
		e_missiles[i].Init();
	}

	e_beam.Init();

	//弾の当たり判定
	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	//スケール分かける
	minpos *= 2;
	maxpos *= 2;

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);

	m_interceptionSphere = std::make_unique<Sphere>(50);//元々半径25

	Head.Init();
	Leftarm.Init();
	Leftfeet.Init();
	Rightarm.Init();
	Rightfeet.Init();

	//スラスターのエフェクトエミッターで作成する
	//エフェクトの座標を指定する
	emitter_point = m_Position;
	emitter_point.y -= 8;
	std::unique_ptr<Emitter> emit = std::make_unique<Emitter>();
	emit->Start(&emitter_point, 200, 
		1.0f,	//円錐の半径
		10.0f,	//円錐の高さ
		23.0f,	//方位角	
		23.0f,	//仰角
		120,	//寿命
		0.0f); //重力
	m_emitter.push_back(std::move(emit));

}



void Enemy_Missile::Update(uint64_t deltatime)
{
	//死んでるなら移動させる
	if (HP <= 0) { m_Position = { 0.0f,-20.0f,0.0f }; return; }

	//Move();//移動処理

	//Timer(deltatime);//時間経過処理

	SpecialAttack(deltatime);

	//弾の更新
	for (int i = 0; i < BulletMaxnum; i++)
	{
		e_missiles[i].Update(deltatime);
	}
	
	Head.Update(deltatime);
	Leftarm.Update(deltatime);
	Rightarm.Update(deltatime);
	Leftfeet.Update(deltatime);
	Rightfeet.Update(deltatime);

	//パーティクルのアップデート
	emitter_point = m_Position;
	emitter_point.y -= 8;
	//ForwardToAngles(EmitterSideAngle, EmitterUpAngle, 0.0f, 0.0f);
	if (m_emitter.size() > 0) {
		//m_emitter[0]->UpdateDirection(EmitterSideAngle, EmitterUpAngle);
		RandomGen rundom;
		m_emitter[0]->UpdateDirection(Forward_vec, rundom.UniformFloat(0.0f,360.0f), 30.0f);
		m_emitter[0]->Update();
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

	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	Head.SetRotation(srt.rot);
	Leftarm.SetRotation(srt.rot);
	Rightarm.SetRotation(srt.rot);
	Leftfeet.SetRotation(srt.rot);
	Rightfeet.SetRotation(srt.rot);
	//姿勢の補完をここでする
	srt.rot.x += 1.55f;
	srt.rot.y += 1.55f;

	Head.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_Neck", srt));
	Leftarm.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_LeftArm", srt));
	Rightarm.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_RightArm", srt));
	Leftfeet.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_LeftFeet", srt));
	Rightfeet.SetPosition(m_meshrenderer.LogBoneWorldPosition("Joint_RightFeet", srt));
}

void Enemy_Missile::LateUpdate(uint64_t deltatime) 
{
	Shot_Flag = false;
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

	//一旦補正つけてみる
	srt.rot.x += 1.55f;
	srt.rot.y += 1.55f;

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	//m_shader.SetGPU();

	if (HP > 0) {
		m_meshrenderer.Draw();//HPがないなら描画しない

		for (int i = 0; i < BulletMaxnum; i++)
		{
			e_missiles[i].Draw();
		}

		if(FIRE_BEAM) e_beam.Draw();
	}

	Vector3 poscop = m_Position;//positionのコピーをとる
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(srt.rot.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(srt.rot.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(srt.rot.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	m_Position += Up_vec * 1.0f;
	m_Position += Right_vec * 0.3f;

	//Vector3 poscop = m_Position;

	//poscop += Up_vec * 1;
	//poscop += Forward_vec * 0.3;
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	m_Position = poscop;//positionは元に戻しておく
	

	Head.Draw();
	Leftarm.Draw();
	Rightarm.Draw();
	Leftfeet.Draw();
	Rightfeet.Draw();
	
	if (collision_hit)
	{
		m_shapecube_col->Draw(transmtx, { 0.6f,0.0f,0.0f,0.5f });
	}
	else
	{
		//m_shapecube_col->Draw(transmtx, { 1.0f,1.0f,1.0f,0.5f });
	}

	//弾丸の検知範囲
#ifdef _DEBUG
	if (Avoidance)
	{
		//m_interceptionSphere->Draw(transmtx, { 0.0,0.0,0.5,0.2 });
	}
	else {
		//m_interceptionSphere->Draw(transmtx, { 1.0,1.0,1.0,0.2 });
	}
#endif
	//姿勢の補完をここでする
	/*m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;*/

	//パーティクルの描画
	if (m_emitter.size() > 0) {
		const std::vector<PARTICLE>& allp = m_emitter[0]->GetParticles();

		for (auto& p : allp) {
			SphereDrawerDraw(1.0f, Color(0.8, 0.8, 0.6, 0.3f), p.pos.x, p.pos.y, p.pos.z);
		}
	}
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
	FIRE_BEAM = false;
	beam_time = 0;
	beamsize = { 0,0,0 };
	for (int i = 0; i < BulletMaxnum; i++)
	{
		e_missiles[i].Reset();
	}
	e_beam.Reset();

	//座標のリセット
	m_Position.z += 50;
	m_Position.y += 10;
}

int Enemy_Missile::GetShaderNum()
{
	if (FIRE_BEAM && beam_time > 400)
	{
		if (beam_time < 500)return 1;
		else if (beam_time < 700)return 2;
		else return 2;
		
	}

	return 0;
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
	//マイナスにすると使用していないレーザーにあたるから上空にセットする
	if (Avoidance) return GM31::GE::Collision::BoundingSphere({ 0,100,0 }, 1);

	return GM31::GE::Collision::BoundingSphere(m_Position, 25);
}

void Enemy_Missile::CreateBullet()
{

	Shot_Flag = true;//射撃フラグを付ける
	if (Bulletnum == BulletMaxnum) Bulletnum = 0;

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	

	Matrix4x4 world = srt.GetMatrix();
	Vector3 forward = world.Forward();
	forward.Normalize();
	//forward *= 3.0f;

	
	e_missiles[Bulletnum].Reset();
	//e_missiles[Bulletnum].SetForward(forward);
	e_missiles[Bulletnum].SetForward(Forward_vec);
	if(Pranter_PE)
	e_missiles[Bulletnum].SetObject(player);
	else e_missiles[Bulletnum].SetObject(Partner);

	Vector3 bulletpos = m_meshrenderer.LogBoneWorldPosition("Shot", srt);

	//指定したボーンがないならオブジェクトの中心座標から球を打つ
	if (bulletpos == Vector3::Zero) bulletpos = m_Position;

	//新しい弾を作る
	//std::unique_ptr<E_Missile> pb = std::make_unique<E_Missile>();
	//pb->SetForward(forward);
	//pb->SetPlayar(player);
	//pb->SetScale(Vector3(1, 1, 1));
	//pb->SetRotation(m_Rotation);
	//pb->SetPosition(bulletpos);

	//e_missiles[Bulletnum].SetScale(Vector3(1, 1, 1));
	//playerをセットした時点で角度はセットしているのでここはいらない
	//e_missiles[Bulletnum].SetRotation(m_Rotation);
	e_missiles[Bulletnum].SetPosition(bulletpos);
	e_missiles[Bulletnum].SetShot(true);
	e_missiles[Bulletnum].priod = 1000;

	//e_missile.push_back(std::move(pb));

	Bulletnum++;
}

void Enemy_Missile::CreateBullet(Vector3 forward)
{
	Shot_Flag = true;//射撃フラグを付ける
	if (Bulletnum == BulletMaxnum) Bulletnum = 0;

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置
	
	//念のためforwardを正規化する
	forward.Normalize();

	e_missiles[Bulletnum].Reset();
	e_missiles[Bulletnum].SetForward(forward);
	if (Pranter_PE)
		e_missiles[Bulletnum].SetObject(player);
	else e_missiles[Bulletnum].SetObject(Partner);

	Vector3 bulletpos = m_meshrenderer.LogBoneWorldPosition("Shot", srt);

	//指定したボーンがないならオブジェクトの中心座標から球を打つ
	if (bulletpos == Vector3::Zero) bulletpos = m_Position;

	e_missiles[Bulletnum].SetCount(0);//補正無し
	e_missiles[Bulletnum].SetmaxTurn(0.0f, 1000.0f);//一定時間立ったら元の追従に戻すようにしたい
	e_missiles[Bulletnum].SetTurn(0.03f);//一定時間立ったら元の追従に戻すようにしたい
	e_missiles[Bulletnum].SetPosition(bulletpos);
	e_missiles[Bulletnum].SetShot(true);
	e_missiles[Bulletnum].priod = 1000;

	//e_missile.push_back(std::move(pb));

	Bulletnum++;
}

void Enemy_Missile::CreateBullet_FullBurst()
{
	if (Bulletnum == BulletMaxnum) Bulletnum = 0;

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置



	Matrix4x4 world = srt.GetMatrix();
	Vector3 forward = world.Forward();
	forward.Normalize();
	forward *= 3.0f;


	e_missiles[Bulletnum].Reset();
	e_missiles[Bulletnum].SetForward(forward);
	if(Pranter_PE) e_missiles[Bulletnum].SetObject(player);
	else e_missiles[Bulletnum].SetObject(Partner);

	//前向き行列取ってから姿勢補完する
	//姿勢補完分
	/*srt.rot.x += 1.55;
	srt.rot.y += 1.55;*/
	Vector3 bulletpos = m_meshrenderer.LogBoneWorldPosition("Shot", srt);

	//指定したボーンがないならオブジェクトの中心座標から球を打つ
	if (bulletpos == Vector3::Zero) bulletpos = m_Position;

	// 右へ30度オフセットした方向
	float angle = 0.0f;
	if(Bulletnum %2 ==1) angle = XMConvertToRadians(90.0f);
	else angle = XMConvertToRadians(-90.0f);
	
	Vector3 dir = forward * cosf(angle) + Right_vec * sinf(angle);
	dir.Normalize();

	e_missiles[Bulletnum].SetForward(dir);

	// 回転角度に変換
	float yaw = atan2f(dir.x, dir.z);
	float pitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
	e_missiles[Bulletnum].SetRotation(Vector3(pitch, yaw, 0.0f));


	//e_missiles[Bulletnum].SetScale(Vector3(1, 1, 1));
	e_missiles[Bulletnum].SetCount(0);//補正無し
	//e_missiles[Bulletnum].SetRotation(m_Rotation);
	e_missiles[Bulletnum].SetPosition(bulletpos);
	e_missiles[Bulletnum].SetShot(true);
	e_missiles[Bulletnum].priod = 1000;

	//デフォルトだと角度の動き悪いので補正する
	//ある程度乱数使って動かす
	RandomGen rand;
	float Turn = 0.015f + rand.UniformFloat(-0.005f, 0.015f);
	float TurnTime = rand.UniformFloat(800.0f, 2200.0f);
	e_missiles[Bulletnum].SetmaxTurn(Turn , TurnTime);//一定時間立ったら元の追従に戻すようにしたい
	float Speed = rand.UniformFloat(0.08f, 0.12f);
	e_missiles[Bulletnum].SetShotSpeed(Speed);
	e_missiles[Bulletnum].ResetVector();
	//e_missile.push_back(std::move(pb));

	Bulletnum++;
}

void Enemy_Missile::CreateBullet_FullBurst_Tes()
{
	if (Bulletnum == BulletMaxnum) Bulletnum = 0;

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	// 親の軸
	Vector3 forward = GetForward();
	Vector3 right = GetRight();
	Vector3 up = GetUp();

	forward.Normalize();
	right.Normalize();
	up.Normalize();

	RandomGen rand;
	//float maxYaw = XMConvertToRadians(35.0f);   // 左右最大15度
	//float maxPitch = XMConvertToRadians(-45.0f);   // 上方向最大45度
	float maxYaw = 0.6108f;   // 左右最大15度
	float maxPitch = -0.7853;   // 上方向最大45度
	//0.6108
	//-0.7853

	float t = (float)Burstnum / 20.0f;            // 0.0～1.0に正規化

	float yawAngle = (Burstnum % 2 == 0) ? -maxYaw : maxYaw; // 左右交互
	float pitchAngle = maxPitch * t;                          // 上方向に段階的

	// Yaw 回転（Up 軸まわり）
	XMVECTOR fwd = XMLoadFloat3(&forward);
	XMVECTOR upv = XMLoadFloat3(&up);
	XMMATRIX yawRot = XMMatrixRotationAxis(upv, yawAngle);
	fwd = XMVector3TransformNormal(fwd, yawRot);

	// Pitch 回転（Right 軸まわり）
	XMVECTOR rightv = XMLoadFloat3(&right);
	XMMATRIX pitchRot = XMMatrixRotationAxis(rightv, pitchAngle);
	fwd = XMVector3TransformNormal(fwd, pitchRot);

	// 最終方向
	Vector3 dir;
	XMStoreFloat3(&dir, fwd);
	dir.Normalize();


	// 弾にセット
	e_missiles[Bulletnum].Reset();
	e_missiles[Bulletnum].SetForward(dir);
	if (Pranter_PE) e_missiles[Bulletnum].SetObject(player);
	else e_missiles[Bulletnum].SetObject(Partner);

	// 回転角度に変換
	float yaw = atan2f(dir.x, dir.z);
	float pitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
	e_missiles[Bulletnum].SetRotation(Vector3(pitch, yaw, 0.0f));

	//e_missiles[Bulletnum].SetScale(Vector3(1, 1, 1));
	e_missiles[Bulletnum].SetCount(0);//補正無し
	//e_missiles[Bulletnum].SetRotation(m_Rotation);
	e_missiles[Bulletnum].SetPosition(m_Position);
	e_missiles[Bulletnum].SetShot(true);
	e_missiles[Bulletnum].priod = 1000;

	//デフォルトだと角度の動き悪いので補正する
	//ある程度乱数使って動かす

	float Turn = 0.025f + rand.UniformFloat(-0.005f, 0.015f);
	//float TurnTime = rand.UniformFloat(800.0f, 2200.0f);
	float TurnTime = rand.UniformFloat(2200.0f, 4200.0f);
	e_missiles[Bulletnum].SetmaxTurn(Turn, TurnTime);//一定時間立ったら元の追従に戻すようにしたい
	float Speed = rand.UniformFloat(0.08f, 0.12f);
	e_missiles[Bulletnum].SetShotSpeed(Speed);
	e_missiles[Bulletnum].ResetVector();
	//e_missile.push_back(std::move(pb));

	Bulletnum++;

}

void Enemy_Missile::Move()//めちゃ雑なルールベース
{
	if (HP <= 0) return;

	//プレイヤーから距離を取るようにする
	Vector3 P_E_Renged = { 0.0f,0.0f,0.0f };
	if(Pranter_PE) P_E_Renged = m_Position - player->GetPosition();
	else P_E_Renged = m_Position - Partner->GetPosition();

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
		if(Pranter_PE) m_Position += player->GetForward() * 0.1f;
		else m_Position += Partner->GetForward() * 0.1f;
		
		if (!FIRE_BEAM) m_Position += Right_vec * 0.3f;//回り込む感じに動く
	}
	else if (P_E_Renged.x + P_E_Renged.y + P_E_Renged.z > 120)//離れすぎたら近づく
	{
		//プレイヤーに近づく
		if(Pranter_PE) m_Position -= player->GetForward() * 0.1f;
		else m_Position -= Partner->GetForward() * 0.1f;
	}

	if (AvoidancePowor != 0)
	{
		m_Position += AvoidanceVec * AvoidancePowor;

		AvoidancePowor -= 0.2f;

		if (AvoidancePowor < 0) AvoidancePowor = 0;
	}

	//ビーム撃ってないなら常にプレイヤーの方へ向く
	if (!FIRE_BEAM) 
	{
		
		Vector3 TargetForward = {0.0f,0.0f,0.0f};
		if(Pranter_PE) TargetForward = (m_Position - player->GetPosition());
		else TargetForward = (m_Position - Partner->GetPosition());

		TargetForward.Normalize();

		// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
		float yaw = atan2f(TargetForward.x, TargetForward.z);
		float pitch = atan2f(-TargetForward.y,
			sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

		// 4. Roll は今回は固定 0
		m_Rotation = Vector3{ 0.0f,yaw, 0.0f };
	}
}

void Enemy_Missile::Move(Vector3 Target)
{
	if (HP <= 0) return;

	//あり得ない値が入っていた場合処理しない

	//Vector3 MoveVec = m_Position - Target;
	
	//これ距離が近すぎるなら動かないようにしたい
	Vector3 coppos_P = {0.0f,0.0f,0.0f};
	if(Pranter_PE) coppos_P = player->GetPosition() + Target;//中間地点を産出する
	else coppos_P = Partner->GetPosition() + Target;

	Vector3 coppos_E = m_Position;
	if (coppos_P.x < 0) coppos_P.x *= -1;
	if (coppos_P.y < 0) coppos_P.y *= -1;
	if (coppos_P.z < 0) coppos_P.z *= -1;
	//あり得ない値が入っているなら処理しない(どこかに1000以上がはいっているなら)
	if (coppos_P.x > 1000 || coppos_P.y > 1000 || coppos_P.z > 1000) return;

	if (coppos_E.x < 0) coppos_E.x *= -1;
	if (coppos_E.y < 0) coppos_E.y *= -1;
	if (coppos_E.z < 0) coppos_E.z *= -1;
	//Y軸は直接移動できないからXZの2軸判定
	float rangedALL = coppos_P.x - coppos_E.x + coppos_P.z - coppos_E.z;
	if (rangedALL < 0) rangedALL * -1;

	Vector3 MoveVec = Target - m_Position;
	MoveVec.y = 0;//正規化前にy軸を切る
	MoveVec.Normalize();

	if (rangedALL < 10) {//中間地点と現在地が近いなら移動しない
		MoveVec = { 0,0,0 };
	}

	//移動部分(レベル別に速度も調節したいのでベクトルに速度を掛ける形にする)
	m_Position += MoveVec * movespead;

	//ステップの速度が乗っているならすべる
	if (AvoidancePowor != 0)
	{
		m_Position += AvoidanceVec * AvoidancePowor;

		AvoidancePowor -= 0.2f;

		if (AvoidancePowor < 0) AvoidancePowor = 0;
	}

	//ビーム撃ってないなら常にプレイヤーの方へ向く
	if (!FIRE_BEAM)
	{

		Vector3 TargetForward = {0.0f,0.0f,0.0f};
		if(Pranter_PE) TargetForward = (m_Position - player->GetPosition());
		else TargetForward = (m_Position - Partner->GetPosition());

		TargetForward.Normalize();

		// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
		float yaw = atan2f(TargetForward.x, TargetForward.z);
		float pitch = atan2f(-TargetForward.y,
			sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

		// 4. Roll は今回は固定 0
		m_Rotation = Vector3{ 0.0f,yaw, 0.0f };
	}
}

void Enemy_Missile::Timer(uint64_t deltatime)
{
	if (HP <= 0) return;

	//経過時間を記録
	float time = static_cast<float>(deltatime) / 1000;

	//タイマー系統
	//衝突判定と無敵時間の処理
	if (collision_hit)
	{
		Invincibility_time += time;

		if (Invincibility_time > 1000)
		{
			collision_hit = false;

			Invincibility_time = 0;
		}

	}

	//ステップ硬直の判定
	if (Avoidance)
	{
		Avoidance_Cooltime += time;

		if (Avoidance_Cooltime > 500)
		{
			//ここオフにすると迎撃しなくなる
			Avoidance = false;
			Avoidance_Cooltime = 0;
		}

	}

	//発射間隔の整理
	if (!FIRE) {
		cooltime += time;
		//経過時間が射撃間隔を上回ったなら射撃する
		if (cooltime > FireRate)
		{
			cooltime = 0;
			FIRE = true;

		}
	}
	
}

void Enemy_Missile::Shot_Rule(uint64_t deltatime)
{
	if (HP <= 0) return;

	//銃弾の発生
	float time_D = static_cast<float>(deltatime) / 1000;

	float time_B = static_cast<float>(deltatime) / 1000;

	beam_time += time_B;

	if (FIRE_BEAM)//Updateにビーム撃つ部分移植したので消す
	{
		//if (beam_time > 500) //ちょっと溜めてからビーム撃つ
		//{
		//	if (beamsize.x < Maxbeamsize.x)beamsize.x += 0.15f;
		//	if (beamsize.y < Maxbeamsize.y)beamsize.y += 0.15f;
		//	if (beamsize.z < Maxbeamsize.z)beamsize.z += 0.25f;
		//	Vector3 beampos = m_Position - (Forward_vec * 5.0f);
		//	e_beam.SetPosition(beampos);
		//	e_beam.SetRotation(m_Rotation);
		//	e_beam.SetScale(beamsize);

		//	e_beam.Update(deltatime);
		//}


		//if (beam_time > 3000)
		//{
		//	FIRE_BEAM = false;

		//	beam_time = 0;

		//	//Stepavoidance();//ステップのテスト
		//}
	}
	else
	{

		if (FIRE)//弾丸を増やす
		{
			cooltime += time_D;
			if (cooltime > 1000)
			{
				cooltime = 0;

				CreateBullet();
			}
		}

		//ビーム撃ってないなら常にプレイヤーの方へ向く
		Vector3 TargetForward = {0.0f,0.0f,0.0f};
		if(Pranter_PE) TargetForward = (m_Position - player->GetPosition());
		else TargetForward = (m_Position - Partner->GetPosition());

		TargetForward.Normalize();

		// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
		float yaw = atan2f(TargetForward.x, TargetForward.z);
		float pitch = atan2f(-TargetForward.y,
			sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

		// 4. Roll は今回は固定 0
		m_Rotation = Vector3{ 0.0f,yaw, 0.0f };

		if (beam_time > 3000)
		{
			FIRE_BEAM = true;
			beam_time = 0;
			//ビームの初期位置を決める
			beamsize = { 0,0,0 };
			Vector3 beampos = m_Position - (Forward_vec * 5.0f);
			e_beam.SetPosition(beampos);
			e_beam.SetRotation(m_Rotation);
			e_beam.SetScale(beamsize);

			e_beam.Update(deltatime);

		}
	}
}

void Enemy_Missile::Shot(uint64_t deltatime)
{
	if (HP <= 0) return;

	//銃弾の発生
	float time_D = static_cast<float>(deltatime) / 1000;

	if (FIRE)//弾丸を増やす
	{
		CreateBullet();
		FIRE = false;
	}
	

	//ビーム撃ってないなら常にプレイヤーの方へ向く
	Vector3 TargetForward = {0.0f,0.0f,0.0f};
	if(Pranter_PE) TargetForward = (m_Position - player->GetPosition());
	else TargetForward = (m_Position - Partner->GetPosition());

	TargetForward.Normalize();

	// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
	float yaw = atan2f(TargetForward.x, TargetForward.z);
	float pitch = atan2f(-TargetForward.y,
		sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

	// 4. Roll は今回は固定 0
	m_Rotation = Vector3{ 0.0f,yaw, 0.0f };

	
}

void Enemy_Missile::Shot(uint64_t deltatime, Vector3 vec)
{
	if (HP <= 0) return;

	//銃弾の発生
	float time_D = static_cast<float>(deltatime) / 1000;

	if (FIRE)//弾丸を増やす
	{
		//0,0,0が入ってるなら正面向いてるか動いていないと仮定して普通に狙う
		if(vec == Vector3::Zero)CreateBullet();
		else CreateBullet(vec);
		FIRE = false;
	}


	//ビーム撃ってないなら常にプレイヤーの方へ向く
	Vector3 TargetForward = { 0.0f,0.0f,0.0f };
	if (Pranter_PE) TargetForward = (m_Position - player->GetPosition());
	else TargetForward = (m_Position - Partner->GetPosition());

	TargetForward.Normalize();

	// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
	float yaw = atan2f(TargetForward.x, TargetForward.z);
	float pitch = atan2f(-TargetForward.y,
		sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

	// 4. Roll は今回は固定 0
	m_Rotation = Vector3{ 0.0f,yaw, 0.0f };
}

void Enemy_Missile::SpecialAttack(uint64_t deltatime)
{
	//ビームだけ照射中にレベル切り替わったときに取り残されないようにする
	if (beam_time != 0 && SpecialFlag) Beam(deltatime);
	else {
		switch (shotstate) {
		case ShotState::Idle:
			//if (FIRE_BEAM) Beam(deltatime);
			if (SpecialFlag) Beam(deltatime);
			break;
		case ShotState::Easy:
			if (SpecialFlag) Beam(deltatime);
			break;
		case ShotState::Normal:
			if (SpecialFlag) FullBurstLv1(deltatime);
			break;
		case ShotState::Hard:
			if (SpecialFlag) FullBurstLv2(deltatime);
			break;
		default:
			SpecialFlag = false;
			break;
		}
	}
	

	
	
}

//一斉射撃を撃つための変数
void Enemy_Missile::FullBurstLv1(uint64_t deltatime)
{
	//銃弾の発生
	float time_D = static_cast<float>(deltatime) / 1000;
	cooltime += time_D;
	if (cooltime > FireRate_FullBurst) {
		cooltime = 0;
		if (Burstnum > 0) {
			CreateBullet_FullBurst();//左右に散らす弾
			Burstnum--;
		}
		else { 
			SpecialFlag = false;
			Burstnum = 20;
		}
		
	}

}

void Enemy_Missile::FullBurstLv2(uint64_t deltatime)
{
	//銃弾の発生
	float time_D = static_cast<float>(deltatime) / 1000;
	cooltime += time_D;
	if (cooltime > FireRate_FullBurst) {
		cooltime = 0;
		if (Burstnum > 0) {
			CreateBullet_FullBurst_Tes();//上方向に散らす弾
			Burstnum--;
		}
		else {
			SpecialFlag = false;
			Burstnum = 20;
		}

	}
}

void Enemy_Missile::Beam(uint64_t deltatime)
{

	float time_B = static_cast<float>(deltatime) / 1000;

	beam_time += time_B;

	if (beam_time > 500) //ちょっと溜めてからビーム撃つ
	{
		if (beamsize.x < Maxbeamsize.x)beamsize.x += 0.15f;
		if (beamsize.y < Maxbeamsize.y)beamsize.y += 0.15f;
		if (beamsize.z < Maxbeamsize.z)beamsize.z += 0.25f;
		Vector3 beampos = m_Position - (Forward_vec * 5.0f);
		e_beam.SetPosition(beampos);
		e_beam.SetRotation(m_Rotation);
		e_beam.SetScale(beamsize);

		e_beam.Update(deltatime);

		FIRE_BEAM = true;
	}


	if (beam_time > 3000)
	{
		FIRE_BEAM = false;

		beam_time = 0;

		SpecialFlag = false;

		//ビームをリセットする
		beamsize = { 0,0,0 };
		Vector3 beampos = m_Position - (Forward_vec * 5.0f);
		e_beam.SetPosition(beampos);
		e_beam.SetRotation(m_Rotation);
		e_beam.SetScale(beamsize);

		e_beam.Update(deltatime);
		//Stepavoidance();//ステップのテスト
	}
}

bool Enemy_Missile::Collision_EN(GM31::GE::Collision::BoundingBoxOBB colobb)
{
	//プレイヤーを構成する要素全てと判定取ってぶつかってたらその時点でtrue返す
	if (GM31::GE::Collision::CollisionOBB(GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(Head.GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(Leftarm.GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(Rightarm.GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(Leftfeet.GetOBB(), colobb)) return true;

	if (GM31::GE::Collision::CollisionOBB(Rightfeet.GetOBB(), colobb)) return true;
	return false;
}

void Enemy_Missile::Stepavoidance(Vector3 bulletpos ,bool StepVec)
{
	if (FIRE_BEAM) return;//ビーム照射中か既にステップ踏んでるなら何もしない
	//敵の弾の位置から向きを割り出す
	Vector3 TargetForward = (m_Position - bulletpos);

	TargetForward.Normalize();

	// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
	float yaw = atan2f(TargetForward.x, TargetForward.z);
	float pitch = atan2f(-TargetForward.y,
		sqrtf(TargetForward.x * TargetForward.x + TargetForward.z * TargetForward.z));

	// 4. Roll は今回は固定 0
	Vector3 avoidance_Rotation = Vector3{ 0.0f,yaw, 0.0f };

	// 方向ベクトル作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(avoidance_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(avoidance_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(avoidance_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	// 方向ベクトル 抽出
	Vector3 AvoidanceRight_vec = { transmtx._11, transmtx._12, transmtx._13 };
	AvoidanceRight_vec.Normalize();
	//一旦普通のステップにしておく
	if(StepVec)	AvoidanceVec = AvoidanceRight_vec;
	else 	AvoidanceVec = -AvoidanceRight_vec;
	AvoidancePowor = 5.0f;
}

void Enemy_Missile::MoveStep(Vector3 Movevec)//正規化した移動ベクトル入れてステップ踏む
{
	AvoidanceVec = Movevec;
	AvoidancePowor = 5.0f;
}

void Enemy_Missile::SetPlayer(M_Player* pl)
{
	player = pl;
}

GM31::GE::Collision::BoundingBoxOBB Enemy_Missile::GetOBB_Bullet(int bulletnum)
{
	return e_missiles[bulletnum].GetOBB();
}

GM31::GE::Collision::BoundingBoxOBB Enemy_Missile::GetOBB_Beam()
{
	if (FIRE_BEAM) return e_beam.GetOBB02();//根本じゃなくてビームの本体部分を参照する

	GM31::GE::Collision::BoundingBoxOBB obb;

	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		{ 0,100,0 },				// 中心座標（ワールド）
		Width,					// 幅
		Height,					// 高さ
		Depth);					// 奥行

	return obb;
}

void Enemy_Missile::SetShotState(int lev)
{
	switch (lev) {
	case 0:
		shotstate = ShotState::Idle;
		break;
	case 1:
		shotstate = ShotState::Easy;
		break;
	case 2:
		shotstate = ShotState::Normal;
		break;
	case 3:
		shotstate = ShotState::Hard;
		break;
	case 4:
		shotstate = ShotState::Hell;
		break;
	case 5:
		shotstate = ShotState::Lunatic;
		break;
	default:
		shotstate = ShotState::Idle;
		break;
	}
}

int Enemy_Missile::GetShotState()
{
	if (shotstate == ShotState::Idle) return 0;
	if (shotstate == ShotState::Easy) return 1;
	if (shotstate == ShotState::Normal) return 2;
	if (shotstate == ShotState::Hard) return 3;
	if (shotstate == ShotState::Hell) return 4;
	if (shotstate == ShotState::Lunatic) return 5;

	return 0;
}

void Enemy_Missile::SetMoveState(int lev)
{
	switch (lev) {
	case 0:
		movestate = MoveState::Idle;
		break;
	case 1:
		movestate = MoveState::Easy;
		break;
	case 2:
		movestate = MoveState::Normal;
		movespead = 0.5f;
		break;
	case 3:
		movestate = MoveState::Hard;
		movespead = 0.8f;
		break;
	case 4:
		movestate = MoveState::Hell;
		break;
	case 5:
		movestate = MoveState::Lunatic;
		break;
	default:
		movestate = MoveState::Idle;
		break;
	}
}

int Enemy_Missile::GetMoveState()
{
	if (movestate == MoveState::Idle) return 0;
	if (movestate == MoveState::Easy) return 1;
	if (movestate == MoveState::Normal) return 2;
	if (movestate == MoveState::Hard) return 3;
	if (movestate == MoveState::Hell) return 4;
	if (movestate == MoveState::Lunatic) return 5;

	return 0;
}

void Enemy_Missile::SetCollision(bool col, int ATK)
{
	if (!col) return;

	if (!collision_hit) {
		HP -= ATK;

		if (HP < 0) HP = 0;
	}


	collision_hit = true;
}

void Enemy_Missile::SetCollision_Bullet(int num, bool col)
{
	e_missiles[num].SetCol(col);
}

Vector3 RotateAroundAxis(const Vector3& v, const Vector3& axis, float angle)
{
	Quaternion q = Quaternion::CreateFromAxisAngle(axis, angle);
	return Vector3::Transform(v, q);
}


void Enemy_Missile::ForwardToAngles(float& elevation, float& azimuth , float sideangle, float upangle)
{
	Vector3 f = Forward_vec;
	f.Normalize();

	float yawOffset = XMConvertToRadians(sideangle); // 左右
	float pitchOffset = XMConvertToRadians(upangle);   // 上下

	// 1. yaw（左右）→ Up 軸で回す
	f = RotateAroundAxis(f, Up_vec, yawOffset);

	// 2. pitch（上下）→ Right 軸で回す
	f = RotateAroundAxis(f, Right_vec, pitchOffset);

	f.Normalize();

	// 最終的な角度を出力
	azimuth = atan2f(f.z, f.x);
	elevation = asinf(f.y);
}

