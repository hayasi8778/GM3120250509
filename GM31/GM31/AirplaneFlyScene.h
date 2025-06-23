#pragma once
#include <memory>
#include <array>
#include "system/IScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"
#include "system/CStaticMesh.h"
#include "system/CStaticMeshRenderer.h"
#include "system/CShader.h"
#include "camera.h"

class AirplaneFlyScene : public IScene
{
	constexpr static uint32_t MODELNUM = 2;			// STATICMESHの個数
public:
	explicit AirplaneFlyScene();
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;
	void debugFreeCamera();
	void debugDirectionalLight();
	void Fly();
private:
	FreeCamera m_camera;										// フリーカメラ
	std::array<std::unique_ptr<CShader>,MODELNUM> m_shaders;	// シェーダー										
	std::array<std::unique_ptr<CStaticMesh>,MODELNUM>	m_staticmeshs;		// STATICMESH
	std::array<std::unique_ptr<CStaticMeshRenderer>,MODELNUM> m_meshrenders;// STATICMESHRENDERER
	std::array<std::unique_ptr<Segment>, 3> m_segments;			// ローカル軸表示用線分

	// 回転角度
	Vector3 m_Rotation={0,0,0};

	// 回転クォータニオン
	Quaternion m_RotationQ={0,0,0,1};

	// 回転行列
	Matrix4x4 m_RotationMtx=Matrix4x4::Identity;

	// 飛行機の位置
	Vector3 m_position ={ 0.0f, 0.0f, 0.0f };
};
