#pragma once
#include	"commontypes.h"
#include	<string>
#include	<vector>
#include	"CMesh.h"
#include	"renderer.h"
#include	"parseobj.h"

class CObj3DMesh : public CMesh {
public:
	void Load(std::string filename);

	void clear() {
		m_vertices.clear();
		m_indices.clear();
		m_materials.clear();
		m_diffusetexturenames.clear();
		m_subsets.clear();
	}

	// 追加 getter
	const std::vector<std::string>& GetMaterialNames()     const { return m_materialNames; }
	const std::vector<std::string>& GetDiffuseTextureNames() const { return m_diffusetexturenames; }
	const std::vector<SUBSET>& GetSubsets()              const { return m_subsets; }
	const std::vector<MATERIAL>& GetMaterials()            const { return m_materials; }

	const std::vector<MATERIAL>& GetMaterials() {
		return m_materials;
	}

	const std::vector<SUBSET>& GetSubsets() {
		return m_subsets;
	}

	const std::vector<std::string>& GetDiffuseTextureNames() {
		return m_diffusetexturenames;
	}
private:
	std::vector<MATERIAL> m_materials;					// マテリアル情報

	std::vector<std::string> m_diffusetexturenames;		// ディフューズテクスチャ名

	std::vector<SUBSET> m_subsets;						// サブセット情報	

	std::vector<std::string> m_materialNames;// 追加 newmtlとテクスチャで名前合わせるために追加

};