#pragma once
#pragma once
#include "system/IScene.h"
#include "system/CSprite.h"
#include "camera.h"
#include "quat.h"

class QuaternionScene : public IScene {
public:
	explicit QuaternionScene();
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;
private:
	Camera m_camera;
	std::unique_ptr<CSprite> m_image;

	//向き的な奴?
	float angle = 0.0f;

	//針の原点
	Vector3 s = { 300,300,0 };

	//原点から回転させるためのクォータニオン
	Quaternion Quat;
	//ベクトルをクォータニオンに変換して回転計算取るためのやつ
	Quaternion vectorQuat;
	//最終的な回転
	Quaternion AnswerQuat;
};