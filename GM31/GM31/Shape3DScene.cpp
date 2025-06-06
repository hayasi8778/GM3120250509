
#include <memory>
#include <string>
#include <array>
#include "Shape3DScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"

/**
 * @brief 3D形状のデバッグUIを表示・制御する
 *
 * ImGuiによるインターフェースを使って、形状の種類・サイズ・回転角を変更できます。
 * 選択された形状に応じて m_shape を再構築し、パラメータを更新します。
 */
void Shape3DScene::debugShape3D()
{
	ImGui::Begin("debugShape3D");

	// ラジオボタンで選択する図形の種類
	std::array<std::string, 5> items = { "sphere", "cube", "cylinder", "cone", "capsule" };
	static int16_t selected = 0;
	static int16_t previous_selected = -1;  ///< 前回選択された図形のインデックス

	// ラジオボタンの描画と選択処理
	for (int i = 0; i < items.size(); i++) {
		if (ImGui::RadioButton(items[i].c_str(), selected == i))
			selected = i;
	}

	// 選択が変更されたときのみ図形の再生成を行う
	if (selected != previous_selected) {
		switch (selected) {
		case 0:
			m_shape = std::make_unique<Sphere>(10.0f);					///< 半径指定で球体を生成
			break;
		case 1:
			m_shape = std::make_unique<Box>(10.0f, 10.0f, 10.0f);		///< 幅・高さ・奥行き指定で箱型を生成
			break;
		case 2:
			m_shape = std::make_unique<Cylinder>(10.0f, 20.0f);			///< 円柱
			break;
		case 3:
			m_shape = std::make_unique<Cone>(10.0f, 20.0f);				///< 円錐
			break;
		case 4:
			m_shape = std::make_unique<Capsule>(10.0f, 20.0f);			///< カプセル
			break;
		default:
			break;
		}
		previous_selected = selected;  ///< 前回の選択を更新
	}

	// 図形サイズのスライダー
	static float radius = 10.0f;
	static float height = 10.0f;
	static float depth = 10.0f;

	ImGui::SliderFloat("size1", &radius, 1.0f, 100.0f);
	ImGui::SliderFloat("size2", &height, 1.0f, 100.0f);
	ImGui::SliderFloat("size3", &depth, 1.0f, 100.0f);

	// サイズ変更を反映
	m_shape->SetSize(radius, height, depth);

	// 回転角の調整（X/Y/Z軸）
	ImGui::SliderFloat("X angle", &m_Rotangle.x, 0.0f, PI);
	ImGui::SliderFloat("Y angle", &m_Rotangle.y, 0.0f, PI);
	ImGui::SliderFloat("Z angle", &m_Rotangle.z, 0.0f, PI);

	ImGui::End();
}

/**
 * @brief Shape3DScene クラスのコンストラクタ
 */
Shape3DScene::Shape3DScene()
{

}

/**
 * @brief シーンの更新処理
 *
 * @param deltatime 前フレームからの経過時間（マイクロ秒単位）
 */
void Shape3DScene::update(uint64_t deltatime)
{

}

/**
 * @brief シーンの描画処理
 *
 * @param deltatime 前フレームからの経過時間（マイクロ秒単位）
 */
void Shape3DScene::draw(uint64_t deltatime)
{
	m_camera.Draw();

	// ワイヤーフレームモードに設定して描画
	Renderer::SetFillMode(D3D11_FILL_WIREFRAME);

	// 回転行列の作成（Yaw-Pitch-Roll順）
	Matrix4x4 mtx = Matrix4x4::CreateFromYawPitchRoll(m_Rotangle.y, m_Rotangle.x, m_Rotangle.z);

	// 図形の描画（半透明白）
	m_shape->Draw(mtx, Color(1, 1, 1, 0.5));
}

/**
 * @brief シーンの初期化処理
 */
void Shape3DScene::init()
{
	// カメラの初期化
	m_camera.Init();

	// デフォルト形状の生成（球体）
	m_shape = std::make_unique<Sphere>(10.0f);

	// デバッグUIの登録
	DebugUI::RedistDebugFunction([this]() {
		debugShape3D();
		});
}

/**
 * @brief シーンの終了・解放処理
 */
void Shape3DScene::dispose()
{

}
