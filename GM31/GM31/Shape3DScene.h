#pragma once

#include "system/IScene.h"
#include "system/C3DShape.h"
#include "camera.h"

/**
 * @brief 3D形状を扱うシーンのクラス
 *
 * このクラスは、ISceneインターフェースを継承し、3Dオブジェクトの初期化、更新、描画、破棄処理を担当します。
 */
class Shape3DScene : public IScene {
public:
    /**
     * @brief Shape3DScene のコンストラクタ
     *
     * シーンの初期状態を構築します。
     */
    explicit Shape3DScene();

    /**
     * @brief シーンの状態を更新する
     *
     * @param deltatime 前フレームからの経過時間（マイクロ秒単位）
     */
    void update(uint64_t deltatime) override;

    /**
     * @brief シーンを描画する
     *
     * @param deltatime 前フレームからの経過時間（マイクロ秒単位）
     */
    void draw(uint64_t deltatime) override;

    /**
     * @brief シーンの初期化処理
     *
     * 3Dオブジェクトやカメラなどのリソースを初期化します。
     */
    void init() override;

    /**
     * @brief シーンのリソース解放処理
     *
     * 動的に確保したリソースを解放します。
     */
    void dispose() override;

    int ChangeScene() override;

    /**
     * @brief ImGuiなどでデバッグ情報を表示する
     *
     * 3Dオブジェクトのデバッグ描画を行います。
     */
    void debugShape3D();

private:
    /**
     * @brief オブジェクトの回転角（ラジアン）
     */
    Vector3 m_Rotangle{};

    /**
     * @brief このシーンで使用するカメラ
     */
    Camera m_camera;

    /**
     * @brief 描画対象の3Dオブジェクト
     */
    std::unique_ptr<C3DShape> m_shape;
};
