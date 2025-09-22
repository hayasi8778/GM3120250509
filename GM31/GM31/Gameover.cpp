#include "Gameover.h"
#include "system/CDirectInput.h"

void Gameover::init()
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

	Result = std::make_unique<CSprite>(20, 20, "assets/texture/Gameover.jpg", uv);
}

void Gameover::update(uint64_t deltatime)
{
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_J))
	{
		SceneFlag = true;
	}
}

void Gameover::draw(uint64_t deltatime)
{
	m_camera.Draw();

	Vector3 rot = { 0,3,3.13 };
	Vector3 pos = { 0,10,0.2f };
	Result->Draw(Vector3{ 9, 5, 1 }, rot, pos);
}

void Gameover::dispose()
{

}

int Gameover::ChangeScene()
{
	if (SceneFlag) {
		SceneFlag = false;
		return 3;
	}

	return 0;
}