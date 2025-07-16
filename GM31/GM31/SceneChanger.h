#pragma once
#include "system/IScene.h"
#include "system/C3DShape.h"
#include <memory>
#include <string>
#include "system/noncopyable.h"

//ロボットの挙動テスト用のシーン
class SceneChanger : public IScene
{
private:
	static std::unordered_map<std::string, std::unique_ptr<IScene>> m_scenes;
	static std::string m_currentSceneName;
public:
	//--- ゲームシーン(デバックのときに外から見れると便利なのでpublic)
	enum SceneKind
	{
		TITLE, //タイトル
		MEC_SCENE, //ゲームシーン
		RESULT //リザルト
	};
	static void SetCurrentScene(std::string);
	static void dispose();
	static void init();
	static void update(uint64_t deltatime);
	static void draw(uint64_t deltatime);

};
