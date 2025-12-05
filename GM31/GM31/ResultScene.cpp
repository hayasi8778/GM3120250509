#include "ResultScene.h"
#include    "system/CDirectInput.h"
#include "Gameover.h"

void ResultScene::init()
{
	// カメラ(3D)の初期化
	m_camera.Init();
	// 画像のUV座標
	Vector2 uv[4] = {
		Vector2(0, 0),
		Vector2(1.0f / 1.0f, 0),
		Vector2(0, 1.0f / 1.0f),
		Vector2(1.0f / 1.0f, 1.0f / 1.0f)
	};

	Result = std::make_unique<CSprite>(20, 20, "assets/texture/Clear.jpg", uv);

	// マテリアル生成
	MATERIAL	mtrl_Screen;
	mtrl_Screen.Ambient = Color(0, 0, 0, 0);
	mtrl_Screen.Diffuse = Color(1, 1, 1, 0.5f);//ここが色なのでこれをいじる
	mtrl_Screen.Emission = Color(0, 0, 0, 0);
	mtrl_Screen.Specular = Color(0, 0, 0, 0);
	mtrl_Screen.Shiness = 0;
	mtrl_Screen.TextureEnable = TRUE;
	m_Fade = std::make_unique<CSprite>(200, 200, "assets/texture/ScreenEfect.png", uv,
		mtrl_Screen);
}

void ResultScene::update(uint64_t deltatime)
{
	//フェード
	if (Fade_Time != 0 && !SceneFlag) { Fade_IN(deltatime); return; }

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_J))
	{
		SceneFlag = true;
	}

	if (SceneFlag) Fade_OUT(deltatime);
}

void ResultScene::draw(uint64_t deltatime)
{
	m_camera.Draw();

	Vector3 rot = { 0,3,3.13 };
	Vector3 pos = { 0,10,0.2f };
	Result->Draw3D(Vector3{ 9, 5, 1 }, rot, pos);

	m_Fade->Draw(Vector3{ 7,5,1 }, Vector3(0, 0, 0), Vector3(650, 340, 0));
}

void ResultScene::dispose()
{

}

int ResultScene::ChangeScene()
{
	if (SceneFlag && Fade_Time == 1000) {
		SceneFlag = false;
		return 3;
	}

	return 0;
}


void ResultScene::Fade_IN(uint64_t deltatime)
{
	float time_D = static_cast<float>(deltatime) / 1000;

	Fade_Time -= time_D;
	if (Fade_Time < 0) {
		Fade_Time = 0;
		Fade_Color = 0;
	}
	else  Fade_Color = float(Fade_Time / 1000.0f);

	//materialに適応して読み込む
	MATERIAL	mtrl_Screen;
	mtrl_Screen.Ambient = Color(0, 0, 0, 0);
	mtrl_Screen.Diffuse = Color(1, 1, 1, Fade_Color);//ここが色なのでこれをいじる
	mtrl_Screen.Emission = Color(0, 0, 0, 0);
	mtrl_Screen.Specular = Color(0, 0, 0, 0);
	mtrl_Screen.Shiness = 0;
	mtrl_Screen.TextureEnable = TRUE;

	m_Fade->SetMaterial(mtrl_Screen);
}

void ResultScene::Fade_OUT(uint64_t deltatime)
{
	float time_D = static_cast<float>(deltatime) / 1000;

	Fade_Time += time_D;
	if (Fade_Time > 1000.0f) Fade_Time = 1000.0f;

	Fade_Color = (Fade_Time / 1000.0f);

	//materialに適応して読み込む
	MATERIAL	mtrl_Screen;
	mtrl_Screen.Ambient = Color(0, 0, 0, 0);
	mtrl_Screen.Diffuse = Color(1, 1, 1, Fade_Color);//ここが色なのでこれをいじる
	mtrl_Screen.Emission = Color(0, 0, 0, 0);
	mtrl_Screen.Specular = Color(0, 0, 0, 0);
	mtrl_Screen.Shiness = 0;
	mtrl_Screen.TextureEnable = TRUE;

	m_Fade->SetMaterial(mtrl_Screen);
}