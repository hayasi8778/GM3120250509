#include "TitleScene.h"
#include    "system/CDirectInput.h"

void TitleScene::init()
{
	// カメラ(3D)の初期化
	m_camera.Init();
	m_camera.SetPosition({ 0.0f,100.0f,-300.0f });
	m_camera.SetLookat({ 0.0f,0.0f,0.0f });
	// 画像のUV座標
	Vector2 uv[4] = {
		Vector2(0, 0),
		Vector2(1.0f / 1.0f, 0),
		Vector2(0, 1.0f / 1.0f),
		Vector2(1.0f / 1.0f, 1.0f / 1.0f)
	};

	//Title = std::make_unique<CSprite>(200, 200, "assets/texture/Title.png", uv);
	Title = std::make_unique<CSprite>(200, 200, "assets/texture/Title_02.png", uv);
	Start = std::make_unique<CSprite>(200, 200, "assets/texture/Start.png", uv);

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

	Enemy_Title.Init();
	m_field = std::make_unique<Field>();
	m_field->Init();
	m_skydome = std::make_unique<Skydome>();
	m_skydome->Init();
}

void TitleScene::update(uint64_t deltatime)
{
	//フェード
	if (Fade_Time != 0 && !SceneFlag) { Fade_IN(deltatime); return; }

	Enemy_Title.Update(deltatime);

	if (StartScaleBool)
	{
		StartScale += 0.005f;
		if (StartScale > 1.15)
		{
			StartScaleBool = false;
		}
	}
	else
	{
		StartScale -= 0.005f;
		if (StartScale < 0.85f)
		{
			StartScaleBool = true;
		}
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_SPACE))
	{
		SceneFlag = true;
	}

	if (SceneFlag) Fade_OUT(deltatime);
	Enemy_Title.LateUpdate(deltatime);
}

void TitleScene::draw(uint64_t deltatime)
{
	m_camera.Draw();
	m_field->Draw();
	m_skydome->Draw();
	Vector3 rot = { 0,3,3.13 };
	Vector3 pos = { 0,10,0.2f };
	Enemy_Title.Draw();
	//Title->Draw(Vector3{ 5,3,1 }, Vector3(0, 0, 0), Vector3(650, 100, 0));//旧タイトルロゴ用
	Title->Draw(Vector3{ 3,1,1 }, Vector3(0, 0, 0), Vector3(650, 100, 0));
	Start->Draw(Vector3{ 3 * StartScale,2 * StartScale,1 }, Vector3(0, 0, 0), Vector3(650, 600, 0));

	m_Fade->Draw(Vector3{ 7,5,1 }, Vector3(0, 0, 0), Vector3(650, 340, 0));
}

void TitleScene::dispose()
{
	m_field->Dispose();
}

int TitleScene::ChangeScene()
{
	if (SceneFlag && Fade_Time ==1000) {
		SceneFlag = false;
		return 1;
	}

	return 0;
}

void TitleScene::Fade_IN(uint64_t deltatime)
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

void TitleScene::Fade_OUT(uint64_t deltatime)
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