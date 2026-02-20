#include	"system/IScene.h"
#include	"scenemanager.h"
#include    "ImageDisplayScene.h"

//こっから授業外で自作したヘッダー
#include "MecScene.h"
#include "TitleScene.h"
#include "ResultScene.h"
#include "Gameover.h"
#include "TutorialScene.h"

std::unordered_map<std::string, std::unique_ptr<IScene>> SceneManager::m_scenes;
std::string SceneManager::m_currentSceneName = "";

// 登録されているシーンを全て破棄する
void SceneManager::Dispose() 
{
	// 登録されているすべてシーンの終了処理
	for (auto& s : m_scenes) 
	{
		s.second->dispose();
	}

	m_scenes.clear();
	m_currentSceneName.clear();
}

void SceneManager::SetCurrentScene(std::string current) 
{
}

void SceneManager::Init()
{
	
	////授業資料
	//m_scenes["EnemyLookatScene"] = std::make_unique<EnemyLookatScene>();
	//m_scenes["EnemyLookatScene"]->init();
	//m_currentSceneName = "EnemyLookatScene";
	

	
	////授業資料
	//m_scenes["ImageDisplayScene"] = std::make_unique<ImageDisplayScene>();
	//m_scenes["ImageDisplayScene"]->init();
	//m_currentSceneName = "ImageDisplayScene";
	
	//自作物
	m_scenes["MecScene"] = std::make_unique<MecScene>();
	m_scenes["MecScene"]->init();

	m_scenes["TutorialScene"] = std::make_unique<TutorialScene>();
	m_scenes["TutorialScene"]->init();

	m_scenes["TitleScene"] = std::make_unique<TitleScene>();
	m_scenes["TitleScene"]->init();
	m_scenes["ResultScene"] = std::make_unique<ResultScene>();
	m_scenes["ResultScene"]->init();
	m_scenes["Gameover"] = std::make_unique<Gameover>();
	m_scenes["Gameover"]->init();

	//m_currentSceneName = "TitleScene";
	m_currentSceneName = "TutorialScene";
}

void SceneManager::Draw(uint64_t deltatime)
{

	// 現在のシーンを描画
	m_scenes[m_currentSceneName]->draw(deltatime);
}

void SceneManager::Update(uint64_t deltatime)
{
	// 現在のシーンを更新
	m_scenes[m_currentSceneName]->update(deltatime);

	//シーン切り替えを検知
	int a = m_scenes[m_currentSceneName]->ChangeScene();

	switch (a)
	{
	case 0://シーン続行

		break;

	case 1:
		m_currentSceneName = "MecScene";
		//m_currentSceneName = "Gameover";
		break;
	case 2:
		m_currentSceneName = "ResultScene";
		break;

	case 3:
		m_currentSceneName = "TitleScene";
		break;
	case 4:
		m_currentSceneName = "Gameover";
		break;
	default:
		m_currentSceneName = "TitleScene";
		break;
	}

}
