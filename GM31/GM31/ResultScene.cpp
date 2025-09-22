#include "ResultScene.h"
#include    "system/CDirectInput.h"
#include "Gameover.h"

void ResultScene::init()
{
	// ƒJƒƒ‰(3D)‚Ì‰Šú‰»
	m_camera.Init();
	// ‰æ‘œ‚ÌUVÀ•W
	Vector2 uv[4] = {
		Vector2(0, 0),
		Vector2(1.0f / 1.0f, 0),
		Vector2(0, 1.0f / 1.0f),
		Vector2(1.0f / 1.0f, 1.0f / 1.0f)
	};

	Result = std::make_unique<CSprite>(20, 20, "assets/texture/Clear.jpg", uv);
}

void ResultScene::update(uint64_t deltatime)
{
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_J))
	{
		SceneFlag = true;
	}
}

void ResultScene::draw(uint64_t deltatime)
{
	m_camera.Draw();

	Vector3 rot = { 0,3,3.13 };
	Vector3 pos = { 0,10,0.2f };
	Result->Draw(Vector3{ 9, 5, 1 }, rot, pos);
}

void ResultScene::dispose()
{

}

int ResultScene::ChangeScene()
{
	if (SceneFlag) {
		SceneFlag = false;
		return 3;
	}

	return 0;
}


