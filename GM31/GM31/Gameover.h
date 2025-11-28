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
	std::unique_ptr<CSprite> Result;//リザルト画面

	void Fade_IN(uint64_t deltatime);
	void Fade_OUT(uint64_t deltatime);
	float Fade_Color = 1.0f;//フェードの色
	float Fade_Time = 5000;//フェードにかかる時間

	std::unique_ptr<CSprite> m_Fade;//フェードに使う画像

};