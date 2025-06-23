#pragma once
#include <memory>
#include <array>
#include "system/IScene.h"
#include "system/C3DShape.h"
#include "system/DebugUI.h"
#include "system/CStaticMeshRenderer.h"
#include "system/CShader.h"
#include "system/collision.h"
#include "system/CObj3DMesh.h"
#include "camera.h"
#include "system/parseobj.h"

class ObjParseScene : public IScene
{
	constexpr static uint32_t MODELNUM = 1;			// STATICMESHの個数
public:
	explicit ObjParseScene();
	void update(uint64_t deltatime) override;
	void draw(uint64_t deltatime) override;
	void init() override;
	void dispose() override;

	void LoadObjMesh(int idx);
	void debugSRT();
	void debugFreeCamera();
	void debugDirectionalLight();
	void debugObjLoad();
private:
	FreeCamera m_camera;									// フリーカメラ
	CShader m_shader;										// シェーダー

	std::unique_ptr<Box> m_box{};							// AABBボックス

	std::unique_ptr<CObj3DMesh>		m_mesh;					// OBJメッシュ	
	std::unique_ptr<CMeshRenderer>	m_meshrenderer;			// メッシュレンダラ	
	std::vector<SUBSET>				m_subsets;				// OBJサブセット情報
	std::vector<MATERIAL>			m_materialdata;			// OBJマテリアルデータ
	std::vector<std::string>		m_diffusetexturenames;	// OBJディフューズテクスチャ名
	std::vector<std::unique_ptr<CMaterial>>	m_materials;	// CMaterial群
	std::vector<std::unique_ptr<CTexture>>	m_textures;		// CTexture群

	std::array<SRT, MODELNUM> m_SRTs{};							// SRT
	std::array<Vector3, MODELNUM> m_scale{};					// スケール
	std::array<std::unique_ptr<Segment>, 3> m_segments;			// ローカル軸表示用線分
};
