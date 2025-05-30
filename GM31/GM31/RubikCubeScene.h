#pragma once
#include <memory>
#include <array>
#include "system/IScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"
#include "Camera.h"

class RubikCubeScene : public IScene
{
	// キューブサイズ
	static constexpr float CUBE_SIZE = 10.0f;
	// ルービックキューブ段数
	static constexpr uint8_t CUBELAYERNUM = 3;

	// キューブの距離(爆発するのに必要)
	float CUBE_DISTANCE = 1.0f;

	//キューブ爆発させる変数
	bool BOMFLAG = false;

	//キューブ回すかのフラグ
	bool cubeRotationFlagBlocX[CUBELAYERNUM][CUBELAYERNUM * CUBELAYERNUM] = {};//最終的にブロック単位で判定取る
	bool cubeRotationFlagBlocY[CUBELAYERNUM][CUBELAYERNUM * CUBELAYERNUM] = {};//最終的にブロック単位で判定取る

	//３軸で回転するようにフラグを作り直す
	bool cubeRotationFlagBloc_X[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM] = {};//最終的にブロック単位で判定取る
	bool cubeRotationFlagBloc_Y[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM] = {};//最終的にブロック単位で判定取る
	bool cubeRotationFlagBloc_XYZ[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM] = {};//各座標ごとに変更

	// 回転角度
	Vector3 m_Rotation{}; 

	//回転クォータニオン
	Quaternion m_RotationQ{};

	//回転テスト用のクォータニオン(Xを軸として段数* 一面あたりのキューブ数)
	Quaternion m_RotationQCop[CUBELAYERNUM][CUBELAYERNUM* CUBELAYERNUM]{};
	
	Quaternion m_RotationQCop_XYZ[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM];

	Quaternion m_RotationQCop_XYZ_copy[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM];//判定用

	// 回転行列
	Matrix4x4 m_RotationMtx{};

	// 無理やりそれぞれの回転行列を取る
	Matrix4x4 m_RotationMtxCop[CUBELAYERNUM][CUBELAYERNUM * CUBELAYERNUM]{};

	Matrix4x4 m_RotationMtxCop_XYZ[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM]{};

	Matrix4x4 m_DrawMtx;

	//回転する際の目標(int3使いたかったけど見つけれなかったからVec3)
	Vector3 m_Rotation_It[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM];

	//クォータニオンで回すのあきらめて色情報を回す
	Color m_Cube_Color[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM];
public:
	explicit RubikCubeScene();
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;
	void debugRubikCubeRotation();
	void debugRubikCubeLocalRotation();
	

	//GPT製のコードを色々試す
	void debugRubikCubeRotationTest();
	void debugRubikCubeRotation_90Rot();

	// キューブの補正用コード
	void UpdateRotationState(char axis, int layer);
private:
	Camera m_camera;									// 固定カメラ
	std::unique_ptr<Box> m_shapecube;					// 立方体
	std::array<std::unique_ptr<Segment>,3> m_segments;	// ローカル軸表示用線分
};
