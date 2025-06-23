#include    <memory>
#include    <string>
#include    "system/commontypes.h"
#include	"system/collision.h"
#include	"ObjParseScene.h"
#include	"system/renderer.h"
#include    "system/parseobj.h"

// 無名名前空間(このファイル内でのみ使用するので)
namespace {
	// ここにOBJファイルのパスを指定
	std::vector<std::string>  g_filename = {
		"assets/model/obj/battery1.obj",
		"assets/model/obj/battery2.obj",
		"assets/model/obj/box.obj",
		"assets/model/obj/bullet.obj",
		"assets/model/obj/cylinder.obj",
		"assets/model/obj/enemy.obj",
		"assets/model/obj/goal.obj",
		"assets/model/obj/player.obj"
	};

	int g_fileindex = 0;	// OBJファイルのインデックス

	// グローバル変数
	std::vector<OBJParse::VERTEX> g_vertices;								// 3D頂点データ
	std::vector<OBJParse::TEXCOORD> g_texcoords;							// テクスチャ座標データ
	std::vector<OBJParse::NORMAL> g_normals;								// 法線データ	
	std::unordered_map<std::string, std::vector<std::vector<OBJParse::VERTEXINFOINDEX>>> g_mtrlfaces;	// マテリアルごとのポリゴンインデックス
	std::unordered_map<std::string, OBJParse::MATERIAL> g_materials;		// マテリアルデータ
	std::vector<std::vector<OBJParse::VERTEXINFOINDEX>> g_polygonindexes;	// ポリゴンインデックス


}

// 平行光源の方向セット
void ObjParseScene::debugDirectionalLight()
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

// SRT
void ObjParseScene::debugSRT()
{
	ImGui::Begin("debug Model SRT");

	for (uint8_t idx = 0; idx < m_SRTs.size(); idx++) {
		std::string str;
		str = "Static Mesh" + std::to_string(idx);

		ImGui::Text(str.c_str());
		ImGui::SliderFloat3((str + std::string(" pos")).c_str(), &m_SRTs[idx].pos.x, -100, 100);
		ImGui::SliderFloat3((str + std::string(" rot")).c_str(), &m_SRTs[idx].rot.x, -PI, PI);
		ImGui::SliderFloat((str + std::string(" scale X")).c_str(), &m_SRTs[idx].scale.x, 0.1f, 10);
		ImGui::SliderFloat((str + std::string(" scale Y")).c_str(), &m_SRTs[idx].scale.y, 0.1f, 10);
		ImGui::SliderFloat((str + std::string(" scale Z")).c_str(), &m_SRTs[idx].scale.z, 0.1f, 10);
	}

	// カメラの位置を極座標からデカルト座標に変換
	ImGui::End();
}

// デバッグフリーカメラ
void ObjParseScene::debugFreeCamera()
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

// オブジェクト選択
void ObjParseScene::debugObjLoad()
{
	ImGui::Begin("debug Obj Load");

	// 選択中のインデックス
	static int current_item = 0;

	// アイテムのリスト
	const char* items[] = { 
		"battery1.obj",
		"battery2.obj",
		"box.obj",
		"bullet.obj",
		"cylinder.obj",
		"enemy.obj",
		"goal.obj",
		"player.obj"
	};

	ImGui::Text("Select a load obj file:");
	if (ImGui::ListBox("##listbox", &current_item, items, IM_ARRAYSIZE(items), 8)) {
		if (current_item != g_fileindex)
		{
			g_fileindex=current_item;

			LoadObjMesh(g_fileindex);	// OBJファイル読み込み

	
			// 既存のデータをクリア	

		}
	}

	ImGui::End();
}

ObjParseScene::ObjParseScene()
{
}

void ObjParseScene::update(uint64_t deltatime)
{

}

void ObjParseScene::draw(uint64_t deltatime)
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

	// 3軸のローカル軸を描画
	for (std::size_t modelno = 0; modelno < MODELNUM; modelno++) {

		// ３Ｄモデルの姿勢情報を表す行列を取得
		Matrix4x4 rotmtx = Matrix4x4::CreateFromYawPitchRoll(
			m_SRTs[modelno].rot.y,		// Y軸回転
			m_SRTs[modelno].rot.x,		// X軸回転
			m_SRTs[modelno].rot.z);		// Z軸回転 

		for (int axisno=0; axisno <3; axisno++)
		{
			rotmtx._41 = m_SRTs[modelno].pos.x;
			rotmtx._42 = m_SRTs[modelno].pos.y;
			rotmtx._43 = m_SRTs[modelno].pos.z;
			m_segments[axisno]->Draw(rotmtx, axiscol[axisno]);
		}
	}

	// シェーダをGPUへセット
	m_shader.SetGPU();

	// MODELの描画
	for (std::size_t i = 0; i < m_SRTs.size(); ++i) {
		SRT transform = m_SRTs[i];
		Matrix4x4 mtx = transform.GetMatrix();
		Renderer::SetWorldMatrix(&mtx);

		m_meshrenderer->BeforeDraw();

		// マテリアル数分ループ
		for (int i = 0; i < m_materials.size(); i++)
		{
			// マテリアルをセット
			m_materials[i]->SetGPU();
			// テクスチャをセット
			m_textures[i]->SetGPU();

			m_meshrenderer->DrawSubset(
				m_subsets[i].IndexNum,							// 描画するインデックス数
				m_subsets[i].IndexBase,							// 最初のインデックスバッファの位置	
				0);												// 頂点バッファの最初から使用
		}

	}
}

void ObjParseScene::LoadObjMesh(int idx) 
{

	m_mesh = std::make_unique<CObj3DMesh>();

	m_subsets.clear();				// OBJサブセット情報
	m_materialdata.clear();			// OBJマテリアルデータ
	m_diffusetexturenames.clear();	// OBJディフューズテクスチャ名
	m_materials.clear();			// CMaterial群
	m_textures.clear();				// CTexture群

	m_mesh->Load(g_filename[idx]);				// OBJファイル読み込み

	m_subsets = m_mesh->GetSubsets();			// サブセット情報取得

	m_materialdata = m_mesh->GetMaterials();	// マテリアル情報取得

	m_diffusetexturenames = m_mesh->GetDiffuseTextureNames();	// ディフューズテクスチャ名取得

	// マテリアル生成
	for (auto& mtrl : m_materialdata)
	{
		m_materials.emplace_back(std::make_unique<CMaterial>());
		m_materials[m_materials.size() - 1]->Create(mtrl);
	}

	// レンダラ生成
	m_meshrenderer = std::make_unique<CMeshRenderer>();
	m_meshrenderer->Init(*m_mesh);

	// テクスチャ生成
	for (auto& texname : m_diffusetexturenames)
	{
		m_textures.emplace_back(std::make_unique<CTexture>());
		if (texname.empty() == false)
		{
			std::string path = "assets/model/obj/";
			m_textures[m_textures.size() - 1]->Load(path + texname);
		}
	}
}

void ObjParseScene::init()
{
	// カメラ(3D)の初期化
	m_camera.Init();

	// シェーダーの初期化
	m_shader.Create(
		"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー

	// Objメッシュ生成
	LoadObjMesh(0);	

	// ローカル軸表示用線分の初期化
	m_segments[0] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(100, 0, 0));
	m_segments[1] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 100, 0));
	m_segments[2] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 0, 100));

	// SRTの設定用
	DebugUI::RedistDebugFunction([this]() {
		debugSRT();
		});

	// デバッグフリーカメラ
	DebugUI::RedistDebugFunction([this]() {
		debugFreeCamera();
		});

	// デバッグLIGHT
	DebugUI::RedistDebugFunction([this]() {
		debugDirectionalLight();
		});

	// OBJファイルの読み込み
	DebugUI::RedistDebugFunction([this]() {
		debugObjLoad();
		});

}

void ObjParseScene::dispose()
{

}