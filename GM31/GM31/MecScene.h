#pragma once
#include <memory>
#include <array>
#include "system/IScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"
#include "Camera.h"
#include "Field.h"
#include "system/CSprite.h"
#include "Skydome.h"

#include "Player_Mec.h"
#include "M_Rock.h"
#include "Gun.h"
#include "Enemy_Missile.h"
#include "EnemyAI.h"

enum UseCamera //カメラの役割
{
	UseCameraFree, 
	UseCameraNormal,
	UseCameraRockOn,
	CAMERA_MAX
};

//ロボットの挙動テスト用のシーン
class MecScene : public IScene 
{
	
public:
	MecScene() = default;
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;
	int ChangeScene() override;

	void PlayerMove();
	void PlayerAdhesion(); //オブジェクトの接合
	void PlayerShot();
	void AddSpeed(float, Vector3);
	void SetSpeed(Vector3);

	void Collision_Hit(); //オブジェクト間の当たり判定

	//ロックオンカーソルの描画
	void RockonUpdate();
	void RockonDraw();
	void UIDraw();
	void CameraFlip();//使用カメラ切り替え

	//デバック用のGUI
	void Debug_Box();
	void debugFreeCamera();
	
	void PlayerMovetes();
private:
	float Test = 1000;

	//重力
	float gravity = -9.8f;

	// 描画の為の情報（見た目に関わる部分）
	CShader			m_Sceneshader;	// 通常のシェーダ
	CShader			m_Monochromeshader;	// シェーダ(モノクロ)
	CShader			m_rgbSpritshader;	// シェーダ(色収差)
	CShader			m_Shadowshader;		//シェーダー(丸影)

	// 箱の大きさ
	static constexpr float CUBE_SIZE = 10.0f;

	constexpr static uint32_t BOXNUM = 2;			// BOXの個数

	aiVector3D minpos;

	aiVector3D maxpos;

	//箱に座標情報があるか分からなかったからポジションを作る
	Vector3 Object_Position = {0.0f,0.0f,0.0f};

	//箱の速度
	Vector3 Object_Speed = { 0.0f,0.0f,0.0f };

	//オブジェクトの角度
	Vector3 m_Destrot = { 0.0f,0.0f,0.0f };

	//減衰率(ステップの後に自動で減速させるため)
	float dampingFactor = 0.95f;


	// 回転行列
	Matrix4x4 m_RotationMtx{};

	int UseCamera = UseCameraRockOn;//どのカメラ使うか
	Camera m_camera;									// 固定カメラ
	Vector3 camRot = Vector3{ 0,0,0 };					//カメラの向き
	FreeCamera m_cameraF;								//デバック用の自由カメラ

	std::unique_ptr<Skydome> m_skydome;							// スカイドーム

	//デバック用の箱
	std::unique_ptr<Box> m_shapecube;					// 立方体
	std::unique_ptr<Box> m_shapecube2;					// 立方体
	std::array<std::unique_ptr<Segment>, 3> m_segments;	// ローカル軸表示用線分

	std::array<SRT, BOXNUM> m_boxSRTs{};					// BOXのSRT
	std::array<Vector3, BOXNUM> m_boxSizes{};				// BOXのサイズ


	std::unique_ptr<Box> m_shapecube_col;					// 立方体
	std::array<SRT, BOXNUM> m_boxSRTs_col{};					// BOXのSRT
	std::array<Vector3, BOXNUM> m_boxSizes_col{};				// BOXのサイズ

	std::unique_ptr<Field> m_field;						// フィールド

	M_Player m_player;//プレイヤーモデル(仮)

	std::vector<std::unique_ptr<Object>> m_objects{};                    //接続可能なオブジェクト群

	EnemyThinking Enemy;												//プレイヤーの動きを受け取って動く敵
	//std::vector<std::unique_ptr<Enemy_Missile>> m_enemys{};               //敵

	//接合中のオブジェクト(未接続のときにnullポインタで返すようにする)	全部位につけれるように用意しておく
	const int ADHESIOINGMAX = 5;
	//銃弾の当たり判定をどうするか決めるまでは銃の保存に使う
	Object* AdhesioingObjects[5] = { nullptr ,nullptr ,nullptr ,nullptr ,nullptr };
	//ロックオン中の敵
	Enemy_Missile* RockonEnemy = nullptr;

	//ロックオンカーソル
	std::unique_ptr<CSprite> m_image;

	std::unique_ptr<CSprite> HP_Player_G;//HPの緑の部分
	std::unique_ptr<CSprite> HP_Player_R;//HPの赤の部分

	std::unique_ptr<CSprite> HP_Enemy_G;//HPの緑の部分
	std::unique_ptr<CSprite> HP_Enemy_R;//HPの赤の部分
};