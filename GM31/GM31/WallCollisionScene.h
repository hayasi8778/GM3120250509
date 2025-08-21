#pragma once
#include <memory>
#include <array>
#include "system/IScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"
#include "system/CStaticMesh.h"
#include "system/CStaticMeshRenderer.h"
#include "system/CShader.h"
#include "system/collision.h"
#include "camera.h"
#include "Field.h"
#include "Player.h"
#include "Skydome.h"

class WallCollisionScene : public IScene
{
public:
	explicit WallCollisionScene();
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;
	int ChangeScene() override;
	void debugFreeCamera();
	void debugDirectionalLight();
	void debugModeSelect();
	void debugBoundingSphere();

	Player* GetPlayer() const { return m_player.get(); }	// プレイヤの取得

	enum MODE {
		NoInterpolation,
		Interpolation
	};
private:
	FreeCamera m_camera;									// フリーカメラ
	std::array<std::unique_ptr<Segment>, 3> m_segments;		// ローカル軸表示用線分
	std::unique_ptr<Field> m_field;							// フィールド
	std::unique_ptr<Player> m_player;						// フィールド
	std::unique_ptr<Skydome> m_skydome;						// スカイドーム
	MODE m_mode = NoInterpolation;
	GM31::GE::Collision::BoundingSphere	m_bsplayer;			// プレイヤのBS
};
