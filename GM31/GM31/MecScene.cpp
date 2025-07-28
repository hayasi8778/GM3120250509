#include "MecScene.h"
#include    <memory>
#include	<iostream>
#include	"system/collision.h"
#include "system/CDirectInput.h"

float VALUE_MOVE_PLAYER = 0.1f;						// キー入力時の移動量
float VALUE_ROTATE_PLAYER = PI * 0.02f;				// キー入力時の回転量
float RATE_ROTATE_PLAYER = 0.40f;					// １フレーム当たりの回転割合
float RATE_MOVE_PLAYER = 0.20f;						// １フレーム当たりの減衰割合
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

	m_objects.push_back(std::make_unique<M_Rock>());

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Init();
		//m_objects[i]->SetAdhesioing(false);
	}

}

void MecScene::update(uint64_t deltatime)
{
	//
	PlayerMove();

	PlayerAdhesion();

	m_player.Update();

	//m_camera.SetLookat(m_boxSRTs[0].pos);
	m_camera.SetLookat(m_player.GetPosition());
}

void MecScene::draw(uint64_t deltatime) 
{
	//m_camera.Draw();

	m_cameraF.Draw();

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

	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_boxSRTs[0].pos);

	Matrix4x4 transmtxtes = m_RotationMtx * Matrix4x4::CreateTranslation(m_boxSRTs[1].pos);

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
}

void::MecScene::dispose() 
{

}

void MecScene::PlayerMove()
{
	bool step = false;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LSHIFT))//ステップだけ個別で判定しておく
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

	/// 位置移動
	m_player.SetPosition(m_player.GetPosition() + Object_Speed);

	//取り付けられているオブジェクトも同時に動かす
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->GetAdhesioing())
		{
			//この接続地点の取り方多分重いから代用案考える
			m_objects[i]->SetPosition(m_player.ConectPos() + Object_Speed);
			m_objects[i]->SetRotation(PL_rotation);
		}
	}

	// 移動量に慣性をかける(減速率)
	Object_Speed += -Object_Speed * RATE_MOVE_PLAYER;
	
	////左右移動
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D))//Dキー
	//{
	//	//std::cout << "Dキー押されたぞ:\n";
	//	if (step)
	//	{
	//		AddSpeed(3, { 1.0f,0.0f,0.0f });
	//	}
	//	else AddSpeed(0.2, { 1.0f,0.0f,0.0f });
	//}
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A))
	//{
	//	//std::cout << "Aキー押されたぞ:\n";
	//	if (step)
	//	{
	//		AddSpeed(3, { -1.0f,0.0f,0.0f });
	//	}
	//	else AddSpeed(0.2, { -1.0f,0.0f,0.0f });
	//}

	////前後移動
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))//Wキー
	//{
	//	//std::cout << "Wキー押されたぞ:\n";
	//	if (step){ AddSpeed(3, { 0.0f,0.0f,1.0f });}
	//	else AddSpeed(0.2, { 0.0f,0.0f,1.0f });
	//}
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))//Sキー
	//{
	//	//std::cout << "Sキー押されたぞ:\n";
	//	if (step) { AddSpeed(3, { 0.0f,0.0f,-1.0f }); }
	//	else AddSpeed(0.2, { 0.0f,0.0f,-1.0f });
	//}

	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_SPACE))//SPACEキー
	//{
	//	//std::cout << "Sキー押されたぞ:\n";
	//	AddSpeed(4, { 0.0f,1.0f,0.0f });
	//}

	////重力
	//
	//if (Box_Speed.y < 0.3f && m_player.GetPosition().y > 0)
	//{
	//	if (Box_Speed.y > -0.3f) 
	//	{
	//		AddSpeed(gravity, { 0.0f,0.6f,0.0f });
	//	}
	//	
	//};
	//
	//if (m_player.GetPosition().y < 0)
	//{
	//	//速度の逆数をかけて消す
	//	AddSpeed(1, { 0.0f,-Box_Speed.y,0.0f });
	//}

	////速度減衰と位置の更新
	//// 速度を減衰させる
	//Box_Speed.x *= dampingFactor;
	//Box_Speed.y *= dampingFactor;
	//Box_Speed.z *= dampingFactor;

	//// 速度が十分に小さくなったら停止
	//if (fabs(Box_Speed.x) < 0.01f) Box_Speed.x = 0.0f;
	//if (fabs(Box_Speed.y) < 0.01f) Box_Speed.y = 0.0f;
	//if (fabs(Box_Speed.z) < 0.01f) Box_Speed.z = 0.0f;

	////地面にめり込むならY軸の加速度を0にして実行
	//if (!m_field->IsFrontSide(m_player.GetPosition() + Box_Speed))//移動後のプレイヤー位置と地面で表裏を判定 
	//{
	//	Box_Speed.y = 0;
	//}

	////プレイヤー座標更新
	//m_player.SetPosition(m_player.GetPosition() + Box_Speed);

	////取り付けられているオブジェクトも同時に動かす
	//for (int i = 0; i < m_objects.size(); i++)
	//{
	//	if (m_objects[i]->GetAdhesioing())
	//	{
	//		m_objects[i]->SetPosition(m_objects[i]->GetPosition() + Box_Speed);
	//	}
	//}
	
}

void MecScene::PlayerAdhesion()
{
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_L))//取り付けテスト処理
	{
		// 1) プレイヤーの位置を取得
		Vector3 playerPos = m_player.GetPosition();

		// 2) もっとも近いオブジェクト探索
		Object* closest = nullptr;
		float minDistSq = std::numeric_limits<float>::max();

		for (auto& uptr : m_objects) {
			Object* obj = uptr.get();

			// プレイヤー自身は除外
			if (obj == &m_player) continue;

			Vector3 pos = obj->GetPosition();
			float dx = pos.x - playerPos.x;
			float dy = pos.y - playerPos.y;
			float dz = pos.z - playerPos.z;

			float distSq = dx * dx + dy * dy + dz * dz;
			if (distSq < minDistSq) {
				minDistSq = distSq;
				closest = obj;
			}

		}

		// 3) 見つかった最も近いオブジェクトにだけ処理を通す
		if (closest) {
			closest->SetAdhesioing(true);
			closest->SetPosition(m_player.ConectPos());
		}


		//m_objects[0]->SetAdhesioing(true);
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_K))//取り付けテスト処理
	{
		m_objects[0]->SetAdhesioing(false);
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
