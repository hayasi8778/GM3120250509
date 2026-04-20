#pragma once

#include	<simplemath.h>
#include	<string>
#include	<vector>
#include	<memory>
#include	"CTexture.h"
#include	"CMesh.h"
#include	"renderer.h"
#include	"AssimpPerse.h"
#include	"C3DShape.h"

// BoneDict エイリアスの定義
using BoneDict = std::unordered_map<std::string, GM31::GE::myAssimp::BONE>;

class CStaticMesh : public CMesh {
public:

	//SimpleMathのToMSの変換ヘルパー
	Matrix4x4 ToSM_Matrix4x4(const aiMatrix4x4& m);

	void Load(std::string filename, std::string texturedirectory="");

	//モデルの行列情報を読み込むためのload
	//void Load(std::string filename, std::string texturedirectory);

	void DrawWithBones(const SRT& meshSrt, const Color& meshColor);

	void DrawBoneSpheresRecursive(
		const aiNode* node,
		const aiMatrix4x4& parentMat,
		const SRT& meshSrt,
		const Color& boneColor,
			  Sphere& boneViz,
		const BoneDict& dict   // using BoneDict = std::unordered_map<std::string,BONE>;
	);

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

	const aiScene* GetScene() 
	{
		return m_pScene;
	}

	// 追加：サブセット／メッシュ毎のワールド行列を取得
	const std::vector<DirectX::SimpleMath::Matrix>&
		GetMeshWorldTransforms() const { return m_MeshWorld; }

	//バイナリの書き込み
	void SaveToBinary(const std::string& binfile);

	//バイナリファイルの読み込み
	void LoadFromBinary(const std::string& binfile);

	//該当ファイルがあるかの確認
	bool FileExists(const std::string& path);

	//Loadの読み込みが.fbxまで行ってるので拡張子を除いたファイル名を抽出する
	std::string GetBaseName(const std::string& path);

	//Asimpでと読み込む関数とバイナリで読み組む関数で分ける
	void LoadWithAssimp(std::string filename, std::string texturedirectory = "");

	const std::vector<VERTEX_3D>& GetVertices() const { return m_vertices; };

	std::vector<std::unique_ptr<CTexture>>& GetDiffuseTextures_move() { return m_diffusetextures; }
	const std::vector<std::unique_ptr<CTexture>>& GetDiffuseTextures() const { return m_diffusetextures; }

	//
	const std::unordered_map<std::string, Matrix4x4>& GetBoneWorldMap() const { return m_BoneWorldMap; }

	//バイナリなどで保存したボーンを返す変数
	const std::vector<BONE> GetBones() { return m_bones; };

private:

	std::vector<MATERIAL> m_materials;					// マテリアル情報
	std::vector<std::string> m_diffusetexturenames;		// ディフューズテクスチャ名
	std::vector<SUBSET> m_subsets;						// サブセット情報

	std::vector<std::unique_ptr<CTexture>>	m_diffusetextures;	// ディフューズテクスチャ群

	// CStaticMesh クラスにメンバを追加
	std::vector<std::vector<GM31::GE::myAssimp::BONE>> m_bonesPerMesh;	//ボーン情報

	Assimp::Importer m_importer;     // メンバに追加
	const aiScene* m_pScene = nullptr;

	// 追加：メッシュ (aiMesh index) 毎のグローバル行列
	std::vector<DirectX::SimpleMath::Matrix> m_MeshWorld;

	//ボーン情報
	std::vector<BONE> m_bones;		

	//バイナリ用の構造体
	struct MeshBinHeader {
		uint32_t vertexCount;
		uint32_t indexCount;
		uint32_t subsetCount;
		uint32_t materialCount;
		uint32_t meshWorldCount;
	};

	//ボーン情報
	std::unordered_map<std::string, Matrix4x4> m_BoneWorldMap;

};