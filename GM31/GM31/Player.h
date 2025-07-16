#pragma once

//#include "system/commontypes.h"
//#include "system/CShader.h"
//#include "system/CStaticMesh.h"
//#include "system/CStaticMeshRenderer.h"
#include "Object.h"

class Player  : public Object{
	//// SRT情報（姿勢情報）
	//Vector3	m_Position = Vector3(0.0f, 0.0f, 0.0f);
	//Vector3	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	//Vector3	m_Scale = Vector3(1.0f, 1.0f, 1.0f);

	//// 描画の為の情報（メッシュに関わる情報）
	//CStaticMeshRenderer	m_meshrenderer;
	//CStaticMesh			m_mesh;							// メッシュデータ

	//// 移動量
	//Vector3	m_Move = { 0.0f,0.0f,0.0f };

	//// 目標回転角度
	//Vector3	m_Destrot = { 0.0f,0.0f,0.0f };

	//// 描画の為の情報（見た目に関わる部分）
	//CShader			m_shader;	// シェーダ
public:
	void Init() override;
	void Update() override;
	void Update2();				// NOInterporation
	void Draw() override;
	void Dispose() override;

	/*void SetPosition(Vector3 pos) { m_Position = pos; }
	void SetRotation(Vector3 rot) { m_Rotation = rot; }
	void SetScale(Vector3 scale) { m_Scale = scale; }

	Vector3 GetPosition() const{ return m_Position; }
	Vector3 GetRotation() const { return m_Rotation;  }
	Vector3 GetScale() const { return m_Scale; }

	SRT GetSRT() const{
		SRT srt;
		srt.pos = m_Position;
		srt.rot = m_Rotation;
		srt.scale = m_Scale;
		return srt;
	}

	const CStaticMesh& GetMesh() {
		return m_mesh;
	}*/
};