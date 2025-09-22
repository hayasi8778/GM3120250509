#pragma once
#include "system/IScene.h"
#include "system/CSprite.h"
#include "camera.h"

class TitleScene : public IScene
{

public:
	TitleScene() = default;
	void init() override;
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void dispose() override;
	int ChangeScene() override;
	void test();

private:
	Camera m_camera;
	bool SceneFlag = false;
	std::unique_ptr<CSprite> Title;//HP‚Ì—Î‚Ì•”•ª
};
