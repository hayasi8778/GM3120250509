#include	"system/IScene.h"
#include	"scenemanager.h"
#include    "ImageDisplayScene.h"
#include    "RubikCubeScene.h"
#include    "Shape3DScene.h"
#include    "CollisionOBBScene.h"
#include    "ObjParseScene.h"
#include    "AirplaneFlyScene.h"
#include    "WallCollisionScene.h"
#include	"EnemyLookatScene.h"
#include	"CarDriveScene.h"

//こっから授業外で自作したヘッダー
#include "MecScene.h"

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
	/*
	//授業資料
	m_scenes["EnemyLookatScene"] = std::make_unique<EnemyLookatScene>();
	m_scenes["EnemyLookatScene"]->init();
	m_currentSceneName = "EnemyLookatScene";
	*/

	
	////授業資料
	//m_scenes["ImageDisplayScene"] = std::make_unique<ImageDisplayScene>();
	//m_scenes["ImageDisplayScene"]->init();
	//m_currentSceneName = "ImageDisplayScene";
	

	
	//自作物
	m_scenes["MecScene"] = std::make_unique<MecScene>();
	m_scenes["MecScene"]->init();
	m_currentSceneName = "MecScene";
	
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
}
