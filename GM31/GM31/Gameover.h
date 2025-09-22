#pragma once
#include "system/IScene.h"
#include "system/CSprite.h"
#include "camera.h"

class Gameover : public IScene
{

public:
	Gameover() = default;
	void init() override;
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void dispose() override;
	int ChangeScene() override;
	void test();

private:
	Camera m_camera;
	bool SceneFlag = false;
	std::unique_ptr<CSprite> Result;//ƒŠƒUƒ‹ƒg‰æ–Ê
};