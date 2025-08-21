#include    <memory>
#include    <string>
#include	"CollisionOBBScene.h"
#include	"system/collision.h"

// 角度から姿勢行列をつくる
void CollisionOBBScene::debugBoxSRT()
{
	ImGui::Begin("debug Box SRT");

	for (uint8_t idx = 0; idx < m_boxSRTs.size(); idx++) {
		std::string str;
		str = "Box" + std::to_string(idx);

		ImGui::Text(str.c_str());
		ImGui::SliderFloat3((str + std::string(" pos")).c_str(), &m_boxSRTs[idx].pos.x, -100, 100);
		ImGui::SliderFloat3((str + std::string(" rot")).c_str(), &m_boxSRTs[idx].rot.x, -100, 100);
		ImGui::SliderFloat3((str + std::string(" size")).c_str(), &m_boxSizes[idx].x, 1, 100);
	}

	// カメラの位置を極座標からデカルト座標に変換
	ImGui::End();
}

// デバッグフリーカメラ
void CollisionOBBScene::debugFreeCamera()
{
	ImGui::Begin("debug Free camera");

	static float radius = 100.0f;
	static Vector3 pos = Vector3(0, 0, radius);
	static Vector3 lookat = Vector3(0, 0, 0);
	static float elevation = -90.0f * PI / 180.0f;
	static float azimuth = PI/2.0f;

	ImGui::SliderFloat("Radius", &radius, 1,800);
	ImGui::SliderFloat("Elevation", &elevation, 0, PI * 2.0f);
	ImGui::SliderFloat("Azimuth", &azimuth, 0, PI*2.0f);

	// カメラの位置を極座標からデカルト座標に変換
	m_camera.SetRadius(radius);
	m_camera.SetElevation(elevation);
	m_camera.SetAzimuth(azimuth);
	m_camera.SetLookat(lookat);

	// カメラの位置を極座標から求める
	m_camera.CalcCameraPosition();

	ImGui::End();
}

CollisionOBBScene::CollisionOBBScene()
{
}

void CollisionOBBScene::update(uint64_t deltatime)
{

}

void CollisionOBBScene::draw(uint64_t deltatime)
{
	m_camera.Draw();

	// 3軸カラー
	Color axiscol[3] = {
		Color(1, 0, 0, 1),
		Color(0, 1, 0, 1),
		Color(0, 1, 1, 1)
	};

	// 3軸のワールド軸を描画
	int cnt = 0;
	for (const auto& seg : m_segments)
	{
		Matrix4x4 mtx = Matrix4x4::Identity;
		seg->Draw(mtx, axiscol[cnt]);
		cnt++;
	}

	// 3軸のローカル軸を描画
	for (std::size_t i = 0; i < m_boxSRTs.size(); ++i) {
		Matrix4x4 rotmtx = Matrix4x4::CreateFromYawPitchRoll(m_boxSRTs[i].rot.y, m_boxSRTs[i].rot.x, m_boxSRTs[i].rot.z);

		cnt = 0;
		for (const auto& seg : m_segments)
		{
			rotmtx._41 = m_boxSRTs[i].pos.x;
			rotmtx._42 = m_boxSRTs[i].pos.y;
			rotmtx._43 = m_boxSRTs[i].pos.z;
			seg->Draw(rotmtx, axiscol[cnt]);
			cnt++;
		}
	}

	// OBB
	std::array<GM31::GE::Collision::BoundingBoxOBB,BOXNUM> obbs;

	// OBB情報をセット
	for (std::size_t i = 0; i < obbs.size(); ++i) {
		obbs[i] = GM31::GE::Collision::SetOBB(
			m_boxSRTs[i].rot,				// 姿勢（回転角度）
			m_boxSRTs[i].pos,				// 中心座標（ワールド）
			m_boxSizes[i].x,				// 幅
			m_boxSizes[i].y,				// 高さ
			m_boxSizes[i].z);				// 奥行
	}

	// BOXの色
	std::array<Color, BOXNUM> colors;

	for (std::size_t i = 0; i < m_boxSRTs.size(); ++i) {
		for (std::size_t j = i + 1; j < m_boxSRTs.size(); ++j) {
			bool sts = GM31::GE::Collision::CollisionOBB(obbs[i], obbs[j]);
			if (sts) 
			{
				colors[i] = Color(1, 0, 0, 0.3f);
				colors[j] = Color(1, 0, 0, 0.3f);
			}
			else {
				colors[i] = Color(1, 1, 1, 0.3f);
				colors[j] = Color(1, 1, 1, 0.3f);
			}
		}
	}

	// BOXの描画
	for (std::size_t i = 0; i < m_boxSRTs.size(); ++i) {
		const auto& transform = m_boxSRTs[i];
		const Vector3 size = m_boxSizes[i];					// サイズ

		m_shapebox->SetSize(size.x, size.y, size.z);		// サイズの設定
		m_shapebox->Draw(transform.GetMatrix(), colors[i]);
	}
}


void CollisionOBBScene::init()
{
	// カメラ(3D)の初期化
	m_camera.Init();

	// ボックスの初期化
	m_shapebox = std::make_unique<Box>(BOXWIDTH, BOXHEIGHT, BOXDEPTH);

	// ボックスサイズの初期化
	m_boxSizes.fill(Vector3(BOXWIDTH, BOXHEIGHT, BOXDEPTH));	// すべての要素をBOXWIDTH,BOXHEIGHT,BOXDEPTH;

	// ローカル軸表示用線分の初期化
	m_segments[0] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(100, 0, 0));
	m_segments[1] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 100, 0));
	m_segments[2] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 0, 100));

	// BOXのSRTの設定用
	DebugUI::RedistDebugFunction([this]() {
		debugBoxSRT();
		});

	// デバッグフリーカメラ
	DebugUI::RedistDebugFunction([this]() {
		debugFreeCamera();
		});

}

void CollisionOBBScene::dispose()
{

}

int CollisionOBBScene::ChangeScene()
{
	return 0;
}