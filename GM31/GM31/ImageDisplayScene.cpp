#include    <memory>
#include	"system/CSprite.h"
#include	"ImageDisplayScene.h"

ImageDisplayScene::ImageDisplayScene()
{
}

void ImageDisplayScene::update(uint64_t deltatime)
{

}

void ImageDisplayScene::draw(uint64_t deltatime)
{
	m_image->Draw(Vector3(1, 1, 1), Vector3(0, 0, 0), Vector3(100, 100, 0));
}

void ImageDisplayScene::init()
{
	// ƒhƒ‰ƒSƒ“UVÀ•W
	Vector2 uv[4] = {
		Vector2(0, 0),
		Vector2(1.0f / 3.0f, 0),
		Vector2(0, 1.0f / 4.0f),
		Vector2(1.0f / 3.0f, 1.0f / 4.0f)
	};

	// ƒhƒ‰ƒSƒ“
	m_image = std::make_unique<CSprite>(200, 200, "assets/texture/dora01.png", uv);

}

void ImageDisplayScene::dispose()
{

}