#pragma once

#include	<simplemath.h>
#include	<string>
#include	<vector>
#include	<memory>
#include	"CTexture.h"
#include	"CMesh.h"
#include	"renderer.h"
#include	"AssimpPerse.h"

class CStaticMesh : public CMesh {
public:
	void Load(std::string filename, std::string texturedirectory="");

	void DrawWithBones(const SRT& meshSrt, const Color& meshColor);

	const std::vector<MATERIAL>& GetMaterials() {
		return m_materials;
	}

	const std::vector<SUBSET>& GetSubsets() {
		return m_subsets;
	}

	const std::vector<std::string>& GetDiffuseTextureNames() {
		return m_diffusetexturenames;
	}

	std::vector<std::unique_ptr<CTexture>> GetDiffuseTextures() {
		return std::move(m_diffusetextures);
	}

	// aiMatrix4x4 を 4x4 float 行列にコピー
	Matrix4x4 ConvertAiToMatrix4x4(const aiMatrix4x4& a)
	{
		return Matrix4x4(
			a.a1, a.a2, a.a3, a.a4,
			a.b1, a.b2, a.b3, a.b4,
			a.c1, a.c2, a.c3, a.c4,
			a.d1, a.d2, a.d3, a.d4
		);
	}


private:

	std::vector<MATERIAL> m_materials;					// マテリアル情報
	std::vector<std::string> m_diffusetexturenames;		// ディフューズテクスチャ名
	std::vector<SUBSET> m_subsets;						// サブセット情報

	std::vector<std::unique_ptr<CTexture>>	m_diffusetextures;	// ディフューズテクスチャ群

	// CStaticMesh クラスにメンバを追加
	std::vector<std::vector<GM31::GE::myAssimp::BONE>> m_bonesPerMesh;	//ボーン情報

};