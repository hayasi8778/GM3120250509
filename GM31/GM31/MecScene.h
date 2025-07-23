#pragma once
#include <memory>
#include <array>
#include "system/IScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"
#include "Camera.h"
#include "Field.h"

#include "Player_Mec.h"
#include "M_Rock.h"

//ロボットの挙動テスト用のシーン
class MecScene : public IScene 
{
	
public:
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;

	void PlayerMove();
	void PlayerAdhesion(); //オブジェクトの接合
	void AddSpeed(float, Vector3);
	void SetSpeed(Vector3);

	//デバック用のGUI
	void Debug_Box();
	void debugFreeCamera();
private:
	//重力
	float gravity = -9.8f;
	// 箱の大きさ
	static constexpr float CUBE_SIZE = 10.0f;

	constexpr static uint32_t BOXNUM = 2;			// BOXの個数

	//箱に座標情報があるか分からなかったからポジションを作る
	Vector3 Box_Position = {0.0f,0.0f,0.0f};

	//箱の速度
	Vector3 Box_Speed = { 0.0f,0.0f,0.0f };

	//減衰率(ステップの後に自動で減速させるため)
	float dampingFactor = 0.95f;


	// 回転行列
	Matrix4x4 m_RotationMtx{};

	Camera m_camera;									// 固定カメラ
	FreeCamera m_cameraF;								//デバック用の自由カメラ
	//デバック用の箱
	std::unique_ptr<Box> m_shapecube;					// 立方体
	std::unique_ptr<Box> m_shapecube2;					// 立方体
	std::array<std::unique_ptr<Segment>, 3> m_segments;	// ローカル軸表示用線分

	std::array<SRT, BOXNUM> m_boxSRTs{};					// BOXのSRT
	std::array<Vector3, BOXNUM> m_boxSizes{};				// BOXのサイズ

	std::unique_ptr<Field> m_field;						// フィールド

	M_Player m_player;//プレイヤーモデル(仮)
	M_Rock m_rock;

	std::vector<std::unique_ptr<Object>> m_objects{};                    //接続可能なオブジェクト群

};