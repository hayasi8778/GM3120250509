#pragma once
#include "system/IScene.h"
#include "system/CSprite.h"
#include "camera.h"
#include "Enemy_Title.h"
#include "Field.h"

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
	std::unique_ptr<CSprite> Title;//タイトル画面の一枚絵
	EnemyThinking_Title Enemy_Title;
	std::unique_ptr<Field> m_field;						// フィールド

	void Fade_IN(uint64_t deltatime);
	void Fade_OUT(uint64_t deltatime);
	float Fade_Color = 1.0f;//フェードの色
	float Fade_Time = 1000;//フェードにかかる時間

	std::unique_ptr<CSprite> m_Fade;//フェードに使う画像
};
