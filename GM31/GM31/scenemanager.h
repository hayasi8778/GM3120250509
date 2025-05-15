#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "system/noncopyable.h"

// ‘O•ûéŒ¾
class IScene;

class SceneManager : NonCopyable{

	static std::unordered_map<std::string, std::unique_ptr<IScene>> m_scenes;
	static std::string m_currentSceneName;

public:
	static void SetCurrentScene(std::string);
	static void Dispose();
	static void Init();
	static void Update(uint64_t deltatime);
	static void Draw(uint64_t deltatime);
};
