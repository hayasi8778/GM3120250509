#include	"system/renderer.h"
#include    "system/DebugUI.h"
#include	"fpscontrol.h"
#include	"system/CDirectInput.h"

#include    "scenemanager.h"

void gameinit() 
{
	// レンダラの初期化
	Renderer::Init();

	// DirectInputの初期化
	CDirectInput::GetInstance().Init(Application::GetHInstance(),
		Application::GetWindow(),
		Application::GetWidth(),
		Application::GetHeight());

	// シーンマネージャーの初期化
	SceneManager::Init();

	// デバッグUIの初期化
	DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext());

}

void gameupdate(uint64_t deltatime)
{
	CDirectInput::GetInstance().GetKeyBuffer();		// キーボードの状態を取得
	CDirectInput::GetInstance().GetMouseState();	// マウスの状態を取得

	// シーンマネージャーのアップデート
	SceneManager::Update(deltatime);
}

void gamedraw(uint64_t deltatime) 
{
	// レンダリング前処理
	Renderer::Begin();

	//シーンマネージャーの処理
	SceneManager::Draw(deltatime);

	// デバッグUIの描画
	DebugUI::Render();

	// レンダリング後処理
	Renderer::End();

}

void gamedispose() 
{
	//シーンマネージャーの終了処理
	SceneManager::Dispose();

	// レンダラの終了処理
	Renderer::Uninit();

}

void gameloop()
{
	uint64_t delta_time = 0;

	// フレームの待ち時間を計算する
	static FPS fpsrate(70);

	// 前回実行されてからの経過時間を計測
	delta_time = fpsrate.CalcDelta();

	// 更新処理、描画処理を呼び出す
	gameupdate(delta_time);
	gamedraw(delta_time);

	// 規定時間までWAIT
	fpsrate.Wait();
}