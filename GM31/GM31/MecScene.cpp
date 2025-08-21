#include "MecScene.h"
#include    <memory>
#include	<iostream>
#include	"system/collision.h"
#include "system/CDirectInput.h"

float VALUE_MOVE_PLAYER = 0.03f;					// キー入力時の移動量
float VALUE_ROTATE_PLAYER = PI * 0.02f;				// キー入力時の回転量
float VALUE_JUMP_PLAYER = 3.80;						//プレイヤーのジャンプ力
float RATE_ROTATE_PLAYER = 0.40f;					// １フレーム当たりの回転割合
float GRAVITY = 0.068;								//重力

void MecScene::init() 
{
	// カメラ(3D)の初期化
	m_camera.Init();
	m_cameraF.Init();

	// フィールドの初期化
	m_field = std::make_unique<Field>();
	m_field->Init();

	// ローカル軸表示用線分の初期化
	m_segments[0] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(CUBE_SIZE * 10, 0, 0));
	m_segments[1] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, CUBE_SIZE * 10, 0));
	m_segments[2] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 0, CUBE_SIZE * 10));

	// ボックスの初期化
	m_shapecube = std::make_unique<Box>(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
	m_shapecube2 = std::make_unique<Box>(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

	

	// ボックスサイズの初期化
	m_boxSizes.fill(Vector3(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE));	// すべての要素をBOXWIDTH,BOXHEIGHT,BOXDEPTH;

	//デバック用GUI一式
	// BOXのSRTの設定用
	DebugUI::RedistDebugFunction([this]() {
		Debug_Box();
		});

	// デバッグフリーカメラ
	DebugUI::RedistDebugFunction([this]() {
		debugFreeCamera();
		});

	m_player.Init();
	//m_rock.Init();

	//m_shapecube_col = std::make_unique<Box>(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

	//M_Rockはプレイヤー参照するから必須
	M_Player* pl = &m_player;

	auto rock = std::make_unique<M_Rock>();
	//オブジェクト分
	rock->SetPlayer(pl);
	m_objects.push_back(std::move(rock));
	m_objects.push_back(std::make_unique<M_Gun>());

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Init();
		//m_objects[i]->SetAdhesioing(false);
	}

	//敵
	auto enemy_missile = std::make_unique<Enemy_Missile>();
	enemy_missile->SetPlayer(pl);
	m_enemys.push_back(std::move(enemy_missile));

	for (int i = 0; i < m_enemys.size(); i++)
	{
		m_enemys[i]->Init();
	}

	// 画像のUV座標
	Vector2 uv[4] = {
		Vector2(0, 0),
		Vector2(1.0f / 1.0f, 0),
		Vector2(0, 1.0f / 1.0f),
		Vector2(1.0f / 1.0f, 1.0f / 1.0f)
	};

	//ロックオンカーソル
	m_image = std::make_unique<CSprite>(20, 20, "assets/texture/Rockon.png", uv);

}

void MecScene::update(uint64_t deltatime)
{
	m_boxSRTs[0].pos = m_objects[0]->GetPosition();

	//座標入れる
	//m_boxSRTs_col[0].pos
	
	if (UseCamera == UseCameraRockOn) PlayerMovetes();
	else PlayerMove();
	

	PlayerAdhesion();

	PlayerShot();

	m_player.Update(deltatime);

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Update(deltatime);
	}

	for (int i = 0; i < m_enemys.size(); i++)
	{
		m_enemys[i]->Update(deltatime);
	}

	//m_camera.SetLookat(m_boxSRTs[0].pos);
	m_camera.SetLookat(m_player.GetPosition());
	Vector3 campos = { 0,0,0 };
	switch (UseCamera)
	{
	case UseCameraNormal:
		campos = Vector3{ m_player.GetPosition()  - (m_player.GetForward() * 50)};
		campos.y += 20;
		m_camera.SetPosition(campos);
		break;
	case UseCameraRockOn:
		// 1. forward ベクトルを計算
		Vector3 camForward = (m_player.GetPosition() - m_enemys[0]->GetPosition());
		campos = Vector3{ m_player.GetPosition() + (camForward) };
		campos.y += 20;
		m_camera.SetPosition(campos);
		//プレイヤー角度も変更
		m_player.SetRotation({ 0,camRot.y,0 });
		break;
	}

	//デバック用のアップデート
	CameraFlip();

	//当たり判定の処理
	Collision_Hit();
}

void MecScene::draw(uint64_t deltatime) 
{

	switch (UseCamera) 
	{
	case UseCameraFree: 
		m_cameraF.Draw();
		break;

	case UseCameraNormal:
		m_camera.Draw();
		break;

	case UseCameraRockOn:
		m_camera.Draw();
		break;
	}

	m_field->Draw();

	//Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(Box_Position);

	// OBB
	std::array<GM31::GE::Collision::BoundingBoxOBB, BOXNUM> obbs;

	// OBB情報をセット
	for (std::size_t i = 0; i < obbs.size(); ++i) {
		obbs[i] = GM31::GE::Collision::SetOBB(
			m_boxSRTs[i].rot,				// 姿勢（回転角度）
			m_boxSRTs[i].pos,				// 中心座標（ワールド）
			m_boxSizes[i].x,				// 幅
			m_boxSizes[i].y,				// 高さ
			m_boxSizes[i].z);				// 奥行
	}

	// BOXの色
	std::array<Color, BOXNUM> colors;

	for (std::size_t i = 0; i < m_boxSRTs.size(); ++i) {
		for (std::size_t j = i + 1; j < m_boxSRTs.size(); ++j) {
			bool sts = GM31::GE::Collision::CollisionOBB(obbs[i], obbs[j]);
			if (sts)
			{
				colors[i] = Color(1, 0, 0, 0.3f);
				colors[j] = Color(1, 0, 0, 0.3f);
			}
			else {
				colors[i] = Color(1, 1, 1, 0.3f);
				colors[j] = Color(1, 1, 1, 0.3f);
			}
		}
	}

	//Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_boxSRTs[0].pos);

	SRT plsrt = m_player.GetSRT();


	plsrt.rot.x += 1.55;
	plsrt.rot.y += 1.55;
	

	//m_player.ModelAABB(minpos, maxpos);

	//Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(plsrt.pos);
	//Matrix4x4 transmtxtes = m_RotationMtx * Matrix4x4::CreateTranslation(m_boxSRTs[1].pos);

	/*m_shapecube->Draw(transmtx, {1.0f,1.0f,1.0f,1.0f});
	m_shapecube2->Draw(transmtxtes, { 1.0f,1.0f,1.0f,1.0f });*/

	/*m_shapecube->Draw(transmtx, colors[0]);
	m_shapecube2->Draw(transmtxtes, colors[1]);*/

	

	m_player.Draw();
	//m_rock.Draw();

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Draw();
	}

	for (int i = 0; i < m_enemys.size(); i++)
	{
		m_enemys[i]->Draw();
	}

	//透過の関係で一番最後(最終的にはプレイヤーと一番近い敵とでフォワードベクトル取ってその向きに出す)
	if(UseCamera == UseCameraRockOn)
	RockonDraw();
}

void::MecScene::dispose() 
{

}

int MecScene::ChangeScene()
{
	return 0;
}

void MecScene::PlayerMove()
{
	bool jumpflag = false;

	bool step = false;
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_LSHIFT))//ステップだけ個別で判定しておく
	{
		step = true;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
		{// 左前移動

			float radian;
			radian = PI * 0.75f;

			Object_Speed.x -= sinf(radian) * VALUE_MOVE_PLAYER;
			Object_Speed.z -= cosf(radian) * VALUE_MOVE_PLAYER;

			// 目標角度をセット
			m_Destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 左後移動

			float radian;
			radian = PI * 0.25f;

			Object_Speed.x -= sinf(radian) * VALUE_MOVE_PLAYER;
			Object_Speed.z -= cosf(radian) * VALUE_MOVE_PLAYER;

			// 目標角度をセット
			m_Destrot.y = radian;
		}
		else
		{// 左移動

			float radian;
			radian = PI * 0.50f;

			Object_Speed.x -= sinf(radian) * VALUE_MOVE_PLAYER;
			Object_Speed.z -= cosf(radian) * VALUE_MOVE_PLAYER;

			// 目標角度をセット
			m_Destrot.y = radian;
		}
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D))
	{
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			// 右前移動

			float radian;
			radian = -PI * 0.75f;

			Object_Speed.x -= sinf(radian) * VALUE_MOVE_PLAYER;
			Object_Speed.z -= cosf(radian) * VALUE_MOVE_PLAYER;

			// 目標角度をセット
			m_Destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 右後移動
			float radian;
			radian = -PI * 0.25f;

			Object_Speed.x -= sinf(radian) * VALUE_MOVE_PLAYER;
			Object_Speed.z -= cosf(radian) * VALUE_MOVE_PLAYER;

			// 目標角度をセット
			m_Destrot.y = radian;
		}
		else
		{// 右移動

			float radian;
			radian = -PI * 0.50f;

			Object_Speed.x -= sinf(radian) * VALUE_MOVE_PLAYER;
			Object_Speed.z -= cosf(radian) * VALUE_MOVE_PLAYER;

			// 目標角度をセット
			m_Destrot.y = radian;
		}
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
	{// 前移動
		float radian;
		radian = PI;

		Object_Speed.x -= sinf(radian) * VALUE_MOVE_PLAYER;
		Object_Speed.z -= cosf(radian) * VALUE_MOVE_PLAYER;

		// 目標角度をセット
		m_Destrot.y = PI;
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
	{// 後移動
		float radian;
		radian = 0.0f;

		Object_Speed.x -= sinf(radian) * VALUE_MOVE_PLAYER;
		Object_Speed.z -= cosf(radian) * VALUE_MOVE_PLAYER;

		// 目標角度をセット
		m_Destrot.y = 0.0f;
	}

	

	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_SPACE))//ジャンプ
	{
		if (m_player.GetPosition().y < 10)
		{
			jumpflag = true;
		}
	}
	

	Vector3 PL_rotation = m_player.GetRotation();

	// 目標角度と現在角度との差分を求める
	float diffrot = m_Destrot.y - PL_rotation.y;
	if (diffrot > PI)
	{
		diffrot -= PI * 2.0f;
	}
	if (diffrot < -PI)
	{
		diffrot += PI * 2.0f;
	}

	// 比率計算
	PL_rotation.y += diffrot * RATE_ROTATE_PLAYER;
	if (PL_rotation.y > PI)
	{
		PL_rotation.y -= PI * 2.0f;
	}
	if (PL_rotation.y < -PI)
	{
		PL_rotation.y += PI * 2.0f;
	}

	m_player.SetRotation(PL_rotation);

	//ステップしているなら速度を加速
	if (step) 
	{
		Object_Speed.x *= 5;
		//Object_Speed.y *= 5;
		Object_Speed.z *= 5;
	}
	
	//重力の適用
	Object_Speed.y -= GRAVITY;

	if (m_player.GetPosition().y < 9) 
	{
		Vector3 p_pos = m_player.GetPosition();
		Object_Speed.y = 0;
		p_pos.y = 9;
		m_player.SetPosition(p_pos);
	}

	if (jumpflag) 
	{
		Object_Speed.y += VALUE_JUMP_PLAYER;
	}
	

	/// 位置移動
	m_player.SetPosition(m_player.GetPosition() + Object_Speed);
	//取り付けられているオブジェクトも同時に動かす

	//この接続地点の取り方多分重いから代用案考える
	if (AdhesioingObject) 
	{
		if (AdhesioingObject->GetAttribute() == JOINABLE && AdhesioingObject->GetAdhesioing()) 
		{
			AdhesioingObject->SetPosition(m_player.ConectPos() + Object_Speed);//場所
			AdhesioingObject->SetRotation(m_player.GetRotation());//角度
		}
		
	}
	//Object_Speed = Object_Speed  - rate;
	
	//減速
	Object_Speed.x *= dampingFactor;
	Object_Speed.y *= dampingFactor;
	Object_Speed.z *= dampingFactor;

	
}

void MecScene::PlayerAdhesion()
{
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_L))//取り付けテスト処理
	{
		if (AdhesioingObject)return;//既に取り付けてあるなら処理しない

		// 1) プレイヤーの位置を取得
		Vector3 playerPos = m_player.GetPosition();

		// 2) もっとも近いオブジェクト探索
		float minDistSq = std::numeric_limits<float>::max();

		for (auto& uptr : m_objects) {
			Object* obj = uptr.get();

			// プレイヤー自身は除外
			if (obj == &m_player) continue;
			// 接続可能オブジェクトじゃない場合も除外
			if (obj->GetAttribute() != JOINABLE) continue;

			Vector3 pos = obj->GetPosition();
			float dx = pos.x - playerPos.x;
			float dy = pos.y - playerPos.y;
			float dz = pos.z - playerPos.z;

			float distSq = dx * dx + dy * dy + dz * dz;
			if (distSq < minDistSq) {
				minDistSq = distSq;
				AdhesioingObject = obj;
			}

		}

		// 3) 見つかった最も近いオブジェクトにだけ処理を通す
		if (AdhesioingObject) {
			AdhesioingObject->SetAdhesioing(true);
			AdhesioingObject->SetPosition(m_player.ConectPos());
			AdhesioingObject->Adhesioing();
		}


		//m_objects[0]->SetAdhesioing(true);
	}
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_K))//取り外しテスト処理
	{
		if (AdhesioingObject) 
		{
			if (AdhesioingObject->GetAttribute() == JOINABLE) 
			{
				AdhesioingObject->SetAdhesioing(false);
				AdhesioingObject = nullptr;
			}
			
		}
	}
}

void MecScene::PlayerShot()
{
	
	// 弾発射
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_N)) {

		//m_player.CreateBullet();

		if (AdhesioingObject) 
		{
			int tes = 100;
		}
		else 
		{
			int test = 100;
		}

		m_player.Action(Vector3{ 0,0,0 });
		m_player.SetTarget(m_enemys[0]->GetPosition_P());

		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->GetAttribute() == JOINABLE && m_objects[i]->GetAdhesioing())
			{
				m_objects[i]->Action(Vector3{0,0,0});
			}
			
		}
	}

	// 敵のアクション
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_P))
	{
		for (int i = 0; i < m_enemys.size(); i++)
		{
			m_enemys[i]->Action(Vector3{ 0,0,0 });
		}
	}


	//モデル表示
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_1))
	{
		m_player.DrawModel = true;
		m_player.DrawBone = false;
	}
	//ボーン表示
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_2))
	{
		m_player.DrawModel = false;
		m_player.DrawBone = true;
	}
	//両方表示
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_3))
	{
		m_player.DrawModel = true;
		m_player.DrawBone = true;
	}
	
}

void MecScene::AddSpeed(float initSpeed, Vector3 Speed)
{
	// 新しい速度を計算
	float newSpeedX = Speed.x * initSpeed;
	float newSpeedY = Speed.y * initSpeed;
	float newSpeedZ = Speed.z * initSpeed;

	// 速度更新時に絶対値で判定し、方向を維持する
	if (fabs(Object_Position.x) < fabs(newSpeedX)) Object_Position.x = newSpeedX;
	if (fabs(Object_Position.y) < fabs(newSpeedY)) Object_Position.y = newSpeedY;
	if (fabs(Object_Position.z) < fabs(newSpeedZ)) Object_Position.z = newSpeedZ;

}

void MecScene::SetSpeed(Vector3 Speed)
{
	Object_Position = Speed;
}

void MecScene::Collision_Hit()
{
	m_player.SetCol(GM31::GE::Collision::CollisionOBB(m_player.GetOBB(), m_enemys[0]->GetOBB_Bullet(0)));

	//m_enemys[0]->SetCollision(GM31::GE::Collision::CollisionOBB(m_enemys[0]->GetOBB(), m_player.GetOBB()));

	for (int i = 0; i < 20; i++) 
	{
		m_enemys[0]->SetCollision(GM31::GE::Collision::CollisionOBB(m_enemys[0]->GetOBB(), m_player.GetOBB_Bullet(i)));
	}
}

void MecScene::RockonDraw()
{
	//座標更新前に角度を更新する
	// 事前に #include <cmath> などが必要
	const float eps = 1e-6f;

	// 1. forward ベクトルを計算
	Vector3 camForward = (m_player.GetPosition() - m_enemys[0]->GetPosition());

	// 2. 動きがある場合のみ回転計算
	if (camForward.LengthSquared() > eps) {
		
	}

	camForward.Normalize();

	// 3. Yaw（Y軸回り）と Pitch（X軸回り）を算出
	float yaw = atan2f(camForward.x, camForward.z);
	float pitch = atan2f(-camForward.y,
		sqrtf(camForward.x * camForward.x + camForward.z * camForward.z));

	// 4. Roll は今回は固定 0
	camRot = Vector3{ pitch, yaw, 0.0f };

	m_image->Draw(Vector3{ 1,1,1 }, camRot, m_enemys[0]->GetPosition() + (camForward * 10.0));
}

void MecScene::CameraFlip()
{
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_0))
	{
		int idx = UseCamera;
		++idx;
		//if (idx < 0) idx = CAMERA_MAX - 1;
		if (idx >= CAMERA_MAX) idx = 0;
		UseCamera = idx;
	}
	
}

void MecScene::Debug_Box()
{
	ImGui::Begin("debug Box SRT");

	for (uint8_t idx = 0; idx < m_boxSRTs.size(); idx++) {
		std::string str;
		str = "Box" + std::to_string(idx);

		ImGui::Text(str.c_str());
		ImGui::SliderFloat3((str + std::string(" pos")).c_str(), &m_boxSRTs[idx].pos.x, -100, 100);
		ImGui::SliderFloat3((str + std::string(" rot")).c_str(), &m_boxSRTs[idx].rot.x, -100, 100);
		ImGui::SliderFloat3((str + std::string(" size")).c_str(), &m_boxSizes[idx].x, 1, 100);
	}

	// カメラの位置を極座標からデカルト座標に変換
	ImGui::End();
}

// デバッグフリーカメラ
void MecScene::debugFreeCamera()
{
	ImGui::Begin("debug Free camera");

	static float radius = 100.0f;
	static Vector3 pos = Vector3(0, 0, radius);
	static Vector3 lookat = Vector3(0, 0, 0);
	static float elevation = -90.0f * PI / 180.0f;
	static float azimuth = PI / 2.0f;

	static Vector3 spherecenter = Vector3(0, 0, 0);

	ImGui::SliderFloat("Radius", &radius, 1, 800);
	ImGui::SliderFloat("Elevation", &elevation, -PI, PI);
	ImGui::SliderFloat("Azimuth", &azimuth, -PI, PI);

	ImGui::SliderFloat3("lookat ", &lookat.x, -100, 100);

	//// カメラの位置を極座標からデカルト座標に変換
	m_cameraF.SetRadius(radius);
	m_cameraF.SetElevation(elevation);
	m_cameraF.SetAzimuth(azimuth);
	m_cameraF.SetLookat(lookat);

	// カメラの位置を極座標から求める
	m_cameraF.CalcCameraPosition();

	ImGui::End();
}

void MecScene::PlayerMovetes()
{
	bool step = CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_LSHIFT);
	bool jumpf = false;

	// ジャンプ判定
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_SPACE)
		&& m_player.GetPosition().y < 10.0f)
	{
		jumpf = true;
	}

	// 左右・前後の入力をローカル座標系で集約
	Vector3 localDir = { 0, 0, 0 };
	auto& DI = CDirectInput::GetInstance();

	if (DI.CheckKeyBuffer(DIK_W)) localDir += { 0, 0, 1 };  // 前
	if (DI.CheckKeyBuffer(DIK_S)) localDir += { 0, 0, -1 };  // 後
	if (DI.CheckKeyBuffer(DIK_A)) localDir += {-1, 0, 0 };  // 左
	if (DI.CheckKeyBuffer(DIK_D)) localDir += { 1, 0, 0 };  // 右

	if (localDir.LengthSquared() > 1e-6f)
		localDir.Normalize();  // 斜め移動時も速度一定

	

	if (localDir.LengthSquared() > 1e-6f)
	{
		localDir.Normalize();

		// 2. ワールド変換（）
		Vector3 worldDir = Vector3{ 0,0,0 };
		if (UseCamera == UseCameraNormal) 
		{
			////プレイヤーの向き基準
			//float yaw = m_player.GetRotation().y;
			//Matrix4x4 rot = Matrix4x4::CreateRotationY(yaw);
			//worldDir = Vector3::Transform(localDir, rot);
			//カメラの向き基準
			float camYaw = 0;
			auto rotMatCam = Matrix4x4::CreateRotationY(camYaw);
			worldDir = Vector3::Transform(localDir, rotMatCam);
		}
		else if (UseCamera == UseCameraRockOn) 
		{
			//カメラの向き基準
			float camYaw = camRot.y;
			auto rotMatCam = Matrix4x4::CreateRotationY(camYaw);
			worldDir = Vector3::Transform(localDir, rotMatCam);
		}
		else //それ以外ならとりあえずカメラに合わせる
		{
			//カメラの向き基準
			float camYaw = 0;
			auto rotMatCam = Matrix4x4::CreateRotationY(camYaw);
			worldDir = Vector3::Transform(localDir, rotMatCam);
		}

		//カメラの向き基準
		float camYaw = camRot.y;
		auto rotMatCam = Matrix4x4::CreateRotationY(camYaw);
		worldDir = Vector3::Transform(localDir, rotMatCam);
		
		// 移動量に乗じる前に反転
		worldDir = -worldDir;

		// 3. ステップ時は加速度UP
		float speedScale = VALUE_MOVE_PLAYER * (step ? 5.0f : 1.0f);
		Object_Speed += worldDir * speedScale;

		// 4. 向き目標を設定（ワールド方向ベクトルから計算）
		m_Destrot.y = atan2f(-worldDir.x, -worldDir.z);
	}

	// 5. 重力・ジャンプ・摩擦 etc.
	Object_Speed.y -= GRAVITY;
	if (m_player.GetPosition().y < 9.0f)
	{
		auto p = m_player.GetPosition();
		p.y = 9.0f;  Object_Speed.y = 0;
		m_player.SetPosition(p);
	}
	if (jumpf) Object_Speed.y += VALUE_JUMP_PLAYER;

	// 6. 回転補間
	Vector3 rot = m_player.GetRotation();
	float diff = m_Destrot.y - rot.y;
	if (diff > PI) diff -= 2 * PI;
	if (diff < -PI) diff += 2 * PI;
	rot.y += diff * RATE_ROTATE_PLAYER;
	if (rot.y > PI) rot.y -= 2 * PI;
	if (rot.y < -PI) rot.y += 2 * PI;
	m_player.SetRotation(rot);

	//ステップ
	if (step) {
		Object_Speed.x *= 5;
		Object_Speed.z *= 5;
		if (Object_Speed.y < 0) //若干急降下気味に降りる方が感触良かったから残す
		{
			Object_Speed.y *= 2.5;
		}
	}

	// 7. 位置更新＋減速
	m_player.SetPosition(m_player.GetPosition() + Object_Speed);

	//この接続地点の取り方多分重いから代用案考える
	if (AdhesioingObject)
	{
		if (AdhesioingObject->GetAttribute() == JOINABLE && AdhesioingObject->GetAdhesioing())
		{
			AdhesioingObject->SetPosition(m_player.ConectPos() + Object_Speed);//場所
			AdhesioingObject->SetRotation(camRot);//角度
		}

	}
	Object_Speed *= dampingFactor;

}
