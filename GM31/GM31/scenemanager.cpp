#include	"system/IScene.h"
#include	"scenemanager.h"
#include    "ImageDisplayScene.h"

std::unordered_map<std::string, std::unique_ptr<IScene>> SceneManager::m_scenes;
std::string SceneManager::m_currentSceneName = "";

// “o˜^‚³‚ê‚Ä‚¢‚éƒV[ƒ“‚ð‘S‚Ä”jŠü‚·‚é
void SceneManager::Dispose() 
{
	// “o˜^‚³‚ê‚Ä‚¢‚é‚·‚×‚ÄƒV[ƒ“‚ÌI—¹ˆ—
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
	m_scenes["ImageDisplayScene"] = std::make_unique<ImageDisplayScene>();
	m_scenes["ImageDisplayScene"]->init();
	m_currentSceneName = "ImageDisplayScene";
}

void SceneManager::Draw(uint64_t deltatime)
{

	// Œ»Ý‚ÌƒV[ƒ“‚ð•`‰æ
	m_scenes[m_currentSceneName]->draw(deltatime);
}

void SceneManager::Update(uint64_t deltatime)
{
	// Œ»Ý‚ÌƒV[ƒ“‚ðXV
	m_scenes[m_currentSceneName]->update(deltatime);
}
