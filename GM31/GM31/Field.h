#pragma once

#include "system/commontypes.h"
#include "system/CVertexBuffer.h"
#include "system/CIndexBuffer.h"
#include "system/CShader.h"
#include "system/CMaterial.h"
#include "system/CTexture.h"

class Field {
	// SRT情報（姿勢情報）
	Vector3	m_Position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3	m_Scale = Vector3(1.0f, 1.0f, 1.0f);

	// 描画の為の情報（メッシュに関わる情報）
	CIndexBuffer				m_IndexBuffer;				// インデックスバッファ
	CVertexBuffer<VERTEX_3D>	m_VertexBuffer;				// 頂点バッファ

	// 描画の為の情報（見た目に関わる部分）
	CShader						m_Shader;					// シェーダー
	CMaterial					m_Material;					// マテリアル
	CTexture					m_Texture;					// テクスチャ

	//地面抜けないために変数を追加する
	Vector3 m_PointOnPlane;  // 任意の頂点位置
	Vector3 m_Normal;        // 平面の法線
public:
	void Init();
	void Draw();
	void Dispose();

	//表裏判定用の関数
	// 平面からの符号付き距離を返す
	float SignedDistance(const Vector3& pos) const {
		return m_Normal.Dot(pos - m_PointOnPlane);
	}

	// 距離の符号で表裏を判定
	bool IsFrontSide(const Vector3& pos) const {
		return SignedDistance(pos) >= 0.0f;
	}

};