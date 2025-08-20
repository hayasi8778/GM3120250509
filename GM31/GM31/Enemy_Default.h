#pragma once

#include "system/commontypes.h"
#include "system/CShader.h"
#include "system/CStaticMesh.h"
#include "system/CStaticMeshRenderer.h"
#include "system/IScene.h"

class Enemy {
	static constexpr	float RATE_ROTATE_ENEMY = 0.01f;		// 回転係数
	static constexpr	float RATE_MOVE_MODEL = 0.20f;			// 移動慣性係数

	// SRT情報（姿勢情報）
	Vector3	m_Position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3	m_Scale = Vector3(1.0f, 1.0f, 1.0f);

	// 描画の為の情報（メッシュに関わる情報）
	CStaticMeshRenderer* m_StaticMeshRenderer;
	CStaticMesh			m_mesh;							// メッシュデータ

	// 目標回転角度
	Vector3	m_Destrot = { 0.0f,0.0f,0.0f };

	// オーナーSCENE
	IScene* m_ownerscene = nullptr;

	// 移動量
	Vector3 m_Move = Vector3(0.0f, 0.0f, 0.0f);

	// スピード
	float m_speed = 0.1f;	// 移動速度

	//壁を回避する最中か	
	bool WallSide = false;
	//壁の長さ
	float Wall_Width = 0.0f;

	//壁衝突前の座標
	Vector3 Oldpos = Vector3(0.0f, 0.0f, 0.0f);
public:
	Enemy(IScene* currentscene)
		: m_StaticMeshRenderer(nullptr),
		m_ownerscene(currentscene) {
	}
	void Init();
	void Update();
	void Draw();
	void Dispose();

	void Remove(Vector3 , float);

	void SetPosition(Vector3 pos) { m_Position = pos; }
	void SetRotation(Vector3 rot) { m_Rotation = rot; }
	void SetScale(Vector3 scale) { m_Scale = scale; }

	void SetMeshRenderer(CStaticMeshRenderer* renderer) { m_StaticMeshRenderer = renderer; }

	Vector3 GetPosition() const { return m_Position; }
	Vector3 GetRotation() const { return m_Rotation; }
	Vector3 GetScale() const { return m_Scale; }
	Vector3 GetMove() const { return m_Move; }


};