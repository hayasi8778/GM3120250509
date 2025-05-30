#pragma once
#include <memory>
#include <array>
#include "system/IScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"
#include "Camera.h"

//ロボットの挙動テスト用のシーン
class MecScene : public IScene 
{
public:
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;

	void PlayerMove();
	void AddSpeed(float, Vector3);
private:
	// 箱の大きさ
	static constexpr float CUBE_SIZE = 10.0f;

	//箱に座標情報があるか分からなかったからポジションを作る
	Vector3 Box_Position = {0.0f,0.0f,0.0f};

	//箱の速度
	Vector3 Box_Speed = { 0.0f,0.0f,0.0f };

	//減衰率(ステップの後に自動で減速させるため)
	float dampingFactor = 0.95f;


	// 回転行列
	Matrix4x4 m_RotationMtx{};

	Camera m_camera;									// 固定カメラ
	std::unique_ptr<Box> m_shapecube;					// 立方体
	std::array<std::unique_ptr<Segment>, 3> m_segments;	// ローカル軸表示用線分

};