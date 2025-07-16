#include "MecScene.h"
#include    <memory>
#include	<iostream>
#include	"system/collision.h"

void MecScene::init() 
{
	// カメラ(3D)の初期化
	m_camera.Init();

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

}

void MecScene::update(uint64_t deltatime)
{
	//
	PlayerMove();

}

void MecScene::draw(uint64_t deltatime) 
{
	m_camera.Draw();

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

	m_shapecube->Draw(transmtx, colors[0]);
	m_shapecube2->Draw(transmtxtes, colors[1]);
}

void::MecScene::dispose() 
{

}

void MecScene::PlayerMove()
{
	bool step = false;
	if (GetAsyncKeyState(VK_SHIFT) & 0x0001)//ステップだけ個別で判定しておく
	{
		step = true;
	}

	//0x8000は今押されているか0x0001なら押した瞬間だけ有効
	//左右移動
	if (GetAsyncKeyState(0x44) & 0x8000)//0x44 = Dキー
	{
		//std::cout << "Dキー押されたぞ:\n";
		if (step)
		{
			AddSpeed(3, { 1.0f,0.0f,0.0f });
		}
		else AddSpeed(0.2, { 1.0f,0.0f,0.0f });
	}
	if (GetAsyncKeyState(0x41) & 0x8000)//0x41 = Aキー
	{
		//std::cout << "Aキー押されたぞ:\n";
		if (step)
		{
			AddSpeed(3, { -1.0f,0.0f,0.0f });
		}
		else AddSpeed(0.2, { -1.0f,0.0f,0.0f });
	}

	//前後移動
	if (GetAsyncKeyState(0x57) & 0x8000)//0x57 = Wキー
	{
		//std::cout << "Wキー押されたぞ:\n";
		if (step){ AddSpeed(3, { 0.0f,0.0f,1.0f });}
		else AddSpeed(0.2, { 0.0f,0.0f,1.0f });
	}
	if (GetAsyncKeyState(0x53) & 0x8000)//0x53 = Sキー
	{
		//std::cout << "Sキー押されたぞ:\n";
		if (step) { AddSpeed(3, { 0.0f,0.0f,-1.0f }); }
		else AddSpeed(0.2, { 0.0f,0.0f,-1.0f });
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)//0x53 = Sキー
	{
		//std::cout << "Sキー押されたぞ:\n";
		AddSpeed(4, { 0.0f,1.0f,0.0f });
	}

	//重力
	
	if (Box_Speed.y < 0.3f && Box_Position.y > 0)
	{
		if (Box_Speed.y > -0.3f) 
		{
			AddSpeed(gravity, { 0.0f,0.6f,0.0f });
		}
		
	};
	
	if (Box_Position.y < 0) 
	{
		Box_Position.y = 0;
		SetSpeed({ 0.0f,0.0f,0.0f });
	}

	//速度減衰と位置の更新
	// 速度を減衰させる
	Box_Speed.x *= dampingFactor;
	Box_Speed.y *= dampingFactor;
	Box_Speed.z *= dampingFactor;

	// 速度が十分に小さくなったら停止
	if (fabs(Box_Speed.x) < 0.01f) Box_Speed.x = 0.0f;
	if (fabs(Box_Speed.y) < 0.01f) Box_Speed.y = 0.0f;
	if (fabs(Box_Speed.z) < 0.01f) Box_Speed.z = 0.0f;

	// 位置を更新
	Box_Position.x += Box_Speed.x;
	Box_Position.y += Box_Speed.y;
	Box_Position.z += Box_Speed.z;

	m_boxSRTs[0].pos.x += Box_Speed.x;
	m_boxSRTs[0].pos.y += Box_Speed.y;
	m_boxSRTs[0].pos.z += Box_Speed.z;
}

void MecScene::AddSpeed(float initSpeed, Vector3 Speed)
{
	// 新しい速度を計算
	float newSpeedX = Speed.x * initSpeed;
	float newSpeedY = Speed.y * initSpeed;
	float newSpeedZ = Speed.z * initSpeed;

	// 速度更新時に絶対値で判定し、方向を維持する
	if (fabs(Box_Speed.x) < fabs(newSpeedX)) Box_Speed.x = newSpeedX;
	if (fabs(Box_Speed.y) < fabs(newSpeedY)) Box_Speed.y = newSpeedY;
	if (fabs(Box_Speed.z) < fabs(newSpeedZ)) Box_Speed.z = newSpeedZ;

}

void MecScene::SetSpeed(Vector3 Speed)
{
	Box_Speed = Speed;
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
