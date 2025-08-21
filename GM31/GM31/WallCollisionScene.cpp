#include    <memory>
#include    <string>
#include    "system/commontypes.h"
#include	"system/collision.h"
#include	"system/renderer.h"
#include    "system/CDirectInput.h"
#include	"system/CPlaneMesh.h"
#include    "system/SphereDrawer.h"
#include    "WallCollisionScene.h"
#include    "Player.h"
#include    "Skydome.h"
#include    "walls.h"

// モード変更
void WallCollisionScene::debugModeSelect()
{

	static int selected = 0;		// 0;nointerpolate 1:interpolate

	ImGui::Begin("debug Mode Select");

	ImGui::RadioButton("NOInterporate", &selected, 0);
	ImGui::RadioButton("Interporate", &selected, 1);

	ImGui::End();

	if (selected == 0) {
		m_mode = MODE::NoInterpolation;
	}
	else {
		m_mode = MODE::Interpolation;
	}
}

// 平行光源の方向セット
void WallCollisionScene::debugDirectionalLight()
{
	static Vector4 direction = Vector4(0.0f, 0.0f,  1.0f, 0.0f); // Z軸+方向に光を当てる	

	ImGui::Begin("debug Directional Light");

	ImGui::SliderFloat3("direction ",&direction.x, -1, 1);
	direction.Normalize();										// 正規化

	LIGHT light{};
	light.Enable = true;
	light.Direction = direction;

	light.Direction.Normalize();
	light.Ambient = Color(0.2f, 0.2f, 0.2f, 1.0f);
	light.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4 Direction = Vector4(direction.x, direction.y,direction.z, 0.0f);
	Renderer::SetLight(light);

	ImGui::End();
}

// デバッグフリーカメラ
void WallCollisionScene::debugFreeCamera()
{
	ImGui::Begin("debug Free camera");

	static float radius = 100.0f;
	static Vector3 pos = Vector3(0, 0, radius);
	static Vector3 lookat = Vector3(0, 0, 0);
	static float elevation = -90.0f * PI / 180.0f;
	static float azimuth = PI/2.0f;

	static Vector3 spherecenter = Vector3(0, 0, 0);	

	ImGui::SliderFloat("Radius", &radius, 1,800);
	ImGui::SliderFloat("Elevation", &elevation, -PI, PI);
	ImGui::SliderFloat("Azimuth", &azimuth, -PI, PI);

	ImGui::SliderFloat3("lookat ", &lookat.x, -100, 100);

	// カメラの位置を極座標からデカルト座標に変換
	m_camera.SetRadius(radius);
	m_camera.SetElevation(elevation);
	m_camera.SetAzimuth(azimuth);
	m_camera.SetLookat(lookat);

	// カメラの位置を極座標から求める
	m_camera.CalcCameraPosition();

	ImGui::End();
}

void WallCollisionScene::debugBoundingSphere()
{

	static int selected = 0;		// 0;nointerpolate 1:interpolate

	ImGui::Begin("debug Bounding Sphere");

	ImGui::Text("BS radius %f", m_bsplayer.radius);
	ImGui::Text("BS local center %f %f %f", m_bsplayer.center.x, m_bsplayer.center.y, m_bsplayer.center.z);

	SRT srt = m_player->GetSRT();
	Matrix4x4 wmtx = srt.GetMatrix();
	Vector3 wcenter = wcenter.Transform(m_bsplayer.center, wmtx);

	ImGui::Text("BS world center %f %f %f", wcenter.x, wcenter.y, wcenter.z);

	ImGui::End();

}

WallCollisionScene::WallCollisionScene()
{
}

void WallCollisionScene::update(uint64_t deltatime)
{
	switch (m_mode) {
	case NoInterpolation:
		m_player->Update2();	// プレイヤの更新
		break;
	case Interpolation:
		m_player->Update(deltatime);	// プレイヤの更新
		break;
	}

	// プレイヤのの境界球の位置を計算
	Vector3 nowpos;	
	SRT srt = m_player->GetSRT();
	Matrix4x4 wmtx = srt.GetMatrix();
	Vector3 bspos = Vector3::Transform(m_bsplayer.center, wmtx);

	// 衝突した壁を探す
	std::vector<WallCollision> collisionwalls;
	collisionwalls = checkWallCollision(
		m_bsplayer.radius,				// 球の半径
		bspos,							// 現在位置 	
		Vector3(0,0,0));				// 速度ベクトル(今の位置でチェック)
}

void WallCollisionScene::draw(uint64_t deltatime)
{
	m_camera.Draw();

	// 3軸カラー
	Color axiscol[3] = {
		Color(1, 0, 0, 1),
		Color(0, 1, 0, 1),
		Color(0, 1, 1, 1)
	};

	// 3軸のワールド軸を描画
	for (int axisno = 0; axisno < 3; axisno++)
	{
		Matrix4x4 rotmtx = Matrix4x4::Identity;
		m_segments[axisno]->Draw(rotmtx, axiscol[axisno]);
	}

	m_skydome->Draw();	// スカイドームの描画
	m_field->Draw();	// フィールドの描画	

	m_player->Draw();	// プレイヤの描画

	// 壁群描画
	DrawWalls();

	// プレイヤBSを描画する
	SRT srt = m_player->GetSRT();
	Color bscolor(1, 1, 1, 0.5f);
	Matrix4x4 wmtx = srt.GetMatrix();
	Vector3 bspos = Vector3::Transform(m_bsplayer.center,wmtx);

	SphereDrawerDraw(m_bsplayer.radius, bscolor,bspos.x, bspos.y, bspos.z);
}

void WallCollisionScene::init()
{
	// カメラ(3D)の初期化
	m_camera.Init();

	// ローカル軸表示用線分の初期化
	m_segments[0] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(100, 0, 0));
	m_segments[1] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 100, 0));
	m_segments[2] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 0, 100));

	// フィールドの初期化
	m_field = std::make_unique<Field>();
	m_field->Init();

	// プレイヤの初期化
	m_player = std::make_unique<Player>();
	m_player->Init();

	// スカイドームの初期化
	m_skydome = std::make_unique<Skydome>();
	m_skydome->Init();

	// 壁群初期化
	initWalls();

	// デバッグフリーカメラ
	DebugUI::RedistDebugFunction([this]() {
		debugFreeCamera();
		});

	// デバッグLIGHT
//	DebugUI::RedistDebugFunction([this]() {
//		debugDirectionalLight();
//		});

	// デバッグMODE SELECT
//	DebugUI::RedistDebugFunction([this]() {
//		debugModeSelect();
//		});


	// デバッグ Bounding Sphere
	DebugUI::RedistDebugFunction([this]() {
		debugBoundingSphere();
	});

	// 境界球を作成する
	std::vector<Vector3> vertices{};

	const CStaticMesh& mesh = m_player->GetMesh();
	const std::vector<VERTEX_3D>& verts = mesh.GetVertices();

	for (auto& vert : verts) {
		Vector3 v;
		vertices.push_back(vert.Position);
	}

	// 境界球データを生成
	SRT srt{};
	m_bsplayer = GM31::GE::Collision::calcBSphere(vertices,srt);
}

void WallCollisionScene::dispose()
{
}

int WallCollisionScene::ChangeScene()
{
	return 0;
}
