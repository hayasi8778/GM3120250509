#include    <memory>
#include    <string>
#include    "system/commontypes.h"
#include	"system/collision.h"
#include	"CarDriveScene.h"
#include	"system/renderer.h"
#include    "system/CDirectInput.h"
#include	"system/CPlaneMesh.h"
#include    "Player.h"
#include    "Skydome.h"
#include    "EnemyLookatScene.h"
#include    "Enemies.h"
#include	"walls.h"
#include    "system/SphereDrawer.h"

// モード変更
void EnemyLookatScene::debugModeSelect()
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
void EnemyLookatScene::debugDirectionalLight()
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
void EnemyLookatScene::debugFreeCamera()
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

EnemyLookatScene::EnemyLookatScene()
{
}

void EnemyLookatScene::update(uint64_t deltatime)
{
	switch (m_mode) {
	case NoInterpolation:
		m_player->Update2();	// プレイヤの更新
		break;
	case Interpolation:
		m_player->Update(deltatime);	// プレイヤの更新
		break;
	}

	// 敵群更新
	UpdateEnemies();

	//壁更新
	UpdateWalls();

	// プレイヤのの境界球の位置を計算
	Vector3 nowpos;
	SRT srt = m_player->GetSRT();
	Matrix4x4 wmtx = srt.GetMatrix();
	Vector3 bspos = Vector3::Transform(m_bsplayer.center, wmtx);

	// 衝突した壁を探す(プレイヤー)
	std::vector<WallCollision> collisionwalls;
	collisionwalls = checkWallCollision(
		m_bsplayer.radius,				// 球の半径
		bspos,							// 現在位置 	
		Vector3(0, 0, 0));				// 速度ベクトル(今の位置でチェック)


	// 敵群更新ループ
	std::vector<SRT> srt_E = GetAllRTS();
	// 壁ヒットフラグも collisionwalls の要素数に合わせて準備するのが安全
	// ここでは最初に暫定的な大きさで確保
	

	for (int i = 0; i < ENEMYMAX; i++)
	{
		// 世界行列・境界球中心の計算
		Matrix4x4 wmtx = srt_E[i].GetMatrix();
		bspos = Vector3::Transform(m_bsenemy[i].center, wmtx);

		// 壁衝突チェック
		collisionwalls = checkWallCollision(
			m_bsenemy[i].radius,
			bspos,
			GetEnemy(i)->GetMove()
		);

		std::vector<WallData> Cop_Walls;

		Cop_Walls.resize(MAXWALLNUM);

		//Getg_Walls(Cop_Walls);

		Cop_Walls = GetWALLS();

		//壁に当たったならめり込まないように
		for (int t = 0; t < Cop_Walls.size(); t++)
		{
			
			if (Cop_Walls[t].hitflag)
			{
				EnemyRemove(i, Cop_Walls[t].Rot, Cop_Walls[t].Width);
			}
			
		}
	}

	//// 敵の境界球の位置を計算
	//std::vector<SRT> srt_E = GetAllRTS();
	//bool wallhitflag[MAXWALLNUM];//壁と敵の衝突判定
	//for (int t = 0; t < MAXWALLNUM - 1; t++) //falseで埋め尽くす
	//{
	//	wallhitflag[t] = false;
	//}
	//for (int i = 0; i < ENEMYMAX - 1; i++)
	//{
	//	wmtx = srt_E[i + 1].GetMatrix();//マトリクスが死んでるっぽい
	//	bspos = Vector3::Transform(m_bsenemy[i].center, wmtx);

	//	// 衝突した壁を探す(敵)
	//	collisionwalls = checkWallCollision(//メモリ読めないのが原因
	//		m_bsenemy[i].radius,				// 球の半径
	//		bspos,							// 現在位置 	
	//		Vector3(0, 0, 0));				// 速度ベクトル(今の位置でチェック)
	//	for (int t = 0; t < collisionwalls.size() - 1; t++) //
	//	{
	//		if (collisionwalls[t].Walldata.hitflag)
	//		{
	//			wallhitflag[1] = true;
	//		}

	//	}

	//}
	////判定を戻す
	//for (int t = 0; t < collisionwalls.size() - 1; t++) //
	//{
	//	collisionwalls[t].Walldata.hitflag = wallhitflag[t];

	//}

}

void EnemyLookatScene::draw(uint64_t deltatime)
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

	// 敵群描画
	DrawEnemies();

	// 壁群描画
	DrawWalls();

	// プレイヤBSを描画する
	SRT srt = m_player->GetSRT();
	Color bscolor(1, 1, 1, 0.5f);
	Matrix4x4 wmtx = srt.GetMatrix();
	Vector3 bspos = Vector3::Transform(m_bsplayer.center, wmtx);

	SphereDrawerDraw(m_bsplayer.radius, bscolor, bspos.x, bspos.y, bspos.z);

	// 敵のBSを描画する
	std::vector<SRT> srt_E = GetAllRTS();
	for (int i = 0; i < ENEMYMAX; i++) 
	{
		Color bscolor_E(1, 1, 1, 0.5f);
		Matrix4x4 wmtx_E = srt_E[i].GetMatrix();
		Vector3 bspos_E = Vector3::Transform(m_bsenemy[i].center, wmtx_E);

		SphereDrawerDraw(m_bsenemy[i].radius, bscolor_E, bspos_E.x, bspos_E.y, bspos_E.z);
	}

}

void EnemyLookatScene::init()
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
	
	// 敵群初期化
	InitEnemies(this);

	// 壁群初期化
	initWalls();

	

	// デバッグフリーカメラ
	DebugUI::RedistDebugFunction([this]() {
		debugFreeCamera();
		});

	// デバッグLIGHT
	DebugUI::RedistDebugFunction([this]() {
		debugDirectionalLight();
		});

	// デバッグMODE SELECT
	DebugUI::RedistDebugFunction([this]() {
		debugModeSelect();
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
	m_bsplayer = GM31::GE::Collision::calcBSphere(vertices, srt);

	for (int i = 0; i < ENEMYMAX; i++) 
	{
		// 境界球を作成する
	std::vector<Vector3> vertices_E{};

	const CStaticMesh& mesh_E = *GetEnemyMesh();
	const std::vector<VERTEX_3D>& verts_E = mesh_E.GetVertices();

	for (auto& vert_E : verts_E) {
		Vector3 v_E;
		vertices_E.push_back(vert_E.Position);
	}

	// 境界球データを生成
	SRT srt_E{};
	m_bsenemy[i] = GM31::GE::Collision::calcBSphere(vertices_E, srt_E);

	}

}

void EnemyLookatScene::dispose()
{
	// 敵群後処理
	DisposeEnemies();

}

int EnemyLookatScene::ChangeScene()
{
	return 0;
}

int EnemyLookatScene::ChangeScene()
{
	return 0;
}
