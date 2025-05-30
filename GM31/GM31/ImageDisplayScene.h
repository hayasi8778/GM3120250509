#pragma once
#include "system/IScene.h"
#include "system/CSprite.h"
#include "camera.h"

class ImageDisplayScene : public IScene {
public:
	explicit ImageDisplayScene();
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;
private:
	Camera m_camera;
	std::unique_ptr<CSprite> m_image;
};
