#pragma once

#include	"CStaticMesh.h"
#include	"CMeshRenderer.h"
#include	"CTexture.h"
#include    "CMaterial.h"

class CStaticMeshRenderer : public CMeshRenderer 
{
	std::vector<SUBSET> m_Subsets;
	std::vector<std::unique_ptr<CTexture>> m_DiffuseTextures;
	std::vector<std::unique_ptr<CMaterial>> m_Materiales;

	std::vector<DirectX::SimpleMath::Matrix> m_Worlds;//ワールド行列
	Microsoft::WRL::ComPtr<ID3D11Buffer>  m_pWorldCB;    // ←追加

	//ボーン情報その2
	struct BoneInfo {
		Matrix4x4 offsetMatrix;      // aiBone::mOffsetMatrix
		Matrix4x4 finalTransform;    // 結果格納用
	};
	std::unordered_map<std::string, int>      boneNameToIndex;
	std::vector<BoneInfo>                     boneInfos;
	const aiScene* m_pScene;              // Assimp シーンへの参照
	//その２終わり
	BoneDict      m_BoneDict;             // ボーン辞書をキャッシュ
	std::unique_ptr<Sphere> m_BoneViz;    // 球デバッグ描画オブジェクト
	ComPtr<ID3D11Buffer> m_pBoneWorldCB;  // ボーン用ワールド行列CB

	void DrawBoneRecursive(
		const aiNode* node,
		const aiMatrix4x4& parentAiM,
		const Color& boneColor,
		const DirectX::SimpleMath::Matrix objWorld,
		const SRT& srt);

	void DrawBoneRecursive(
		const aiNode* node,
		const aiMatrix4x4& parentAiM,
		const Color& boneColor,
		SRT  srt);

	DirectX::SimpleMath::Vector3 m_Modelrot = { 0.0f,0.0f,0.0f };//モデルへの補正角

	// ② 再帰ヘルパー：見つかったら true を返して探索を打ち切る
	bool FindAndLogBoneRecursive(const aiNode* node,const aiMatrix4x4& parentAiM,const std::string& targetName,
		const SRT& srt,Vector3& outPos);

public:	
	void Init(CStaticMesh& mesh);
	void Draw();
	// オブジェクト SRT を受け取るオーバーロードを追加
	void Draw(const SRT& objectSrt);
	void DrawWithBones(SRT  srt, const Color& meshColor);

	DirectX::SimpleMath::Vector3 GetModelRot() { return m_Modelrot; };

	// ① 公開メソッド：ターゲットボーン名を指定してログを出力
	Vector3 LogBoneWorldPosition(const std::string& targetName,const SRT& srt);

};
