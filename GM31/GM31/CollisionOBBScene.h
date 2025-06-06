#pragma once
#include <memory>
#include <array>
#include "system/IScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"
#include "camera.h"

class CollisionOBBScene : public IScene
{
	constexpr static float BOXWIDTH = 10.0f;		// BOXの幅
	constexpr static float BOXHEIGHT = 10.0f;		// BOXの高さ
	constexpr static float BOXDEPTH = 10.0f;		// BOXの奥行（Z)
	constexpr static uint32_t BOXNUM = 2;			// BOXの個数
public:
	explicit CollisionOBBScene();
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;
	void debugBoxSRT();
	void debugFreeCamera();

private:
	FreeCamera m_camera;									// フリーカメラ
	std::unique_ptr<Box> m_shapebox{};						// BOX
	std::array<SRT, BOXNUM> m_boxSRTs{};					// BOXのSRT
	std::array<Vector3, BOXNUM> m_boxSizes{};				// BOXのサイズ
	std::array<std::unique_ptr<Segment>, 3> m_segments;		// ローカル軸表示用線分
};
