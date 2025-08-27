#include    "commontypes.h"
#include	"CStaticMesh.h"
#include	<iostream>
#include <assimp/scene.h>        // aiMatrix4x4, aiVector3D, aiNode など
#include    "CDumpMatrix.h"

#include <assimp/config.h>      // ← これが最重要
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

// aiMatrix4x4 → SimpleMath::Matrix 変換ヘルパー
static DirectX::SimpleMath::Matrix ToSM(const aiMatrix4x4& m)
{
	//return DirectX::SimpleMath::Matrix(
	//	m.a1, m.b1, m.c1, m.d1,
	//	m.a2, m.b2, m.c2, m.d2,
	//	m.a3, m.b3, m.c3, m.d3,
	//	m.a4, m.b4, m.c4, m.d4
	//);
    return DirectX::SimpleMath::Matrix(
        // 1行目
        m.a1, m.a2, m.a3, m.a4,
        // 2行目
        m.b1, m.b2, m.b3, m.b4,
        // 3行目
        m.c1, m.c2, m.c3, m.c4,
        // 4行目
        m.d1, m.d2, m.d3, m.d4
    );


}

void DumpAiMatrix(const aiMatrix4x4& A)
{
    printf("%8.4f %8.4f %8.4f %8.4f\n", A.a1, A.a2, A.a3, A.a4);
    printf("%8.4f %8.4f %8.4f %8.4f\n", A.b1, A.b2, A.b3, A.b4);
    printf("%8.4f %8.4f %8.4f %8.4f\n", A.c1, A.c2, A.c3, A.c4);
    printf("%8.4f %8.4f %8.4f %8.4f\n\n", A.d1, A.d2, A.d3, A.d4);
};

void DumpAiMatrix(const aiMatrix4x4& A, char const* tag)
{
    printf("%s (aiMatrix4x4) =\n", tag);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", A.a1, A.a2, A.a3, A.a4);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", A.b1, A.b2, A.b3, A.b4);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", A.c1, A.c2, A.c3, A.c4);
    printf(" %8.4f %8.4f %8.4f %8.4f\n\n", A.d1, A.d2, A.d3, A.d4);
}

void DumpSMMatrix(const DirectX::SimpleMath::Matrix& M, char const* tag)
{
    DirectX::XMFLOAT4X4 f;
    XMStoreFloat4x4(&f, M);  // f._11…_44 にコピー

    printf("%s (SimpleMath::Matrix) =\n", tag);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", f._11, f._12, f._13, f._14);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", f._21, f._22, f._23, f._24);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", f._31, f._32, f._33, f._34);
    printf(" %8.4f %8.4f %8.4f %8.4f\n\n", f._41, f._42, f._43, f._44);
}


void CStaticMesh::Load(std::string filename, std::string texturedirectory)
{
    m_importer.SetPropertyInteger(AI_CONFIG_IMPORT_COLLADA_IGNORE_UP_DIRECTION, 0);

    // 1) assimp でシーン読み込み
    m_pScene = m_importer.ReadFile(
        filename.c_str(),
        aiProcessPreset_TargetRealtime_MaxQuality |
        aiProcess_ConvertToLeftHanded |
        aiProcess_PopulateArmatureData 
    );
    assert(m_pScene);

    // 2) myAssimpWrapper 経由で一次データ取得
    GM31::GE::myAssimp::GetModelData(filename, texturedirectory);
    auto subsets = GM31::GE::myAssimp::GetSubsets();
    auto vertices = GM31::GE::myAssimp::GetVertices();
    auto indices = GM31::GE::myAssimp::GetIndices();
    auto materials = GM31::GE::myAssimp::GetMaterials();
    m_diffusetextures = GM31::GE::myAssimp::GetDiffuseTextures();

    // 3) クラスメンバにコピー（元の処理）
    // ───────────────────────────────────
    // 頂点データ作成
    for (const auto& mv : vertices)
    {
        for (auto& v : mv)
        {
            VERTEX_3D vertex{};
            vertex.Position = { v.pos.x,    v.pos.y,     v.pos.z };
            vertex.Normal = { v.normal.x, v.normal.y,  v.normal.z };
            vertex.TexCoord = { v.texcoord.x,v.texcoord.y };
            vertex.Diffuse = { v.color.r,   v.color.g,    v.color.b, v.color.a };

            vertex.bonecnt = v.bonecnt;
            for (int i = 0; i < v.bonecnt; ++i)
            {
                vertex.BoneIndex[i] = v.BoneIndex[i];
                vertex.BoneWeight[i] = v.BoneWeight[i];
            }
            m_vertices.emplace_back(vertex);
        }
    }

    // インデックスデータ作成
    for (const auto& mi : indices)
        for (auto idx : mi)
            m_indices.emplace_back(idx);

    // サブセットデータ作成
    for (const auto& sub : subsets)
    {
        SUBSET s{};
        s.VertexBase = sub.VertexBase;
        s.VertexNum = sub.VertexNum;
        s.IndexBase = sub.IndexBase;
        s.IndexNum = sub.IndexNum;
        s.MtrlName = sub.mtrlname;
        s.MaterialIdx = sub.materialindex;
        m_subsets.emplace_back(s);
    }

    // マテリアルデータ作成
    for (const auto& m : materials)
    {
        MATERIAL mat{};
        mat.Ambient = { m.Ambient.r, m.Ambient.g, m.Ambient.b, m.Ambient.a };
        mat.Diffuse = { m.Diffuse.r, m.Diffuse.g, m.Diffuse.b, m.Diffuse.a };
        mat.Specular = { m.Specular.r, m.Specular.g, m.Specular.b, m.Specular.a };
        mat.Emission = { m.Emission.r, m.Emission.g, m.Emission.b, m.Emission.a };
        mat.Shiness = m.Shiness;
        mat.TextureEnable = m.diffusetexturename.empty() ? FALSE : TRUE;
        m_materials.emplace_back(mat);
        m_diffusetexturenames.emplace_back(m.diffusetexturename);
    }
    // ───────────────────────────────────

    // 4) 追加：ノード階層を再帰走査して meshIndex→ワールド行列を構築
    m_MeshWorld.resize(m_pScene->mNumMeshes);

    /*std::function<void(const aiNode*, const aiMatrix4x4&)> dfs =
        [&](const aiNode* node, const aiMatrix4x4& parentMat)
        {
            aiMatrix4x4 global = parentMat * node->mTransformation;
            for (unsigned mi = 0; mi < node->mNumMeshes; ++mi)
            {
                unsigned meshIndex = node->mMeshes[mi];
                m_MeshWorld[meshIndex] = ToSM(global);
            }
            for (unsigned i = 0; i < node->mNumChildren; ++i)
                dfs(node->mChildren[i], global);
        };*/

    auto dumpAi = [&](const aiMatrix4x4& A) {
        printf(" %f %f %f %f\n", A.a1, A.a2, A.a3, A.a4);
        printf(" %f %f %f %f\n", A.b1, A.b2, A.b3, A.b4);
        printf(" %f %f %f %f\n", A.c1, A.c2, A.c3, A.c4);
        printf(" %f %f %f %f\n", A.d1, A.d2, A.d3, A.d4);
        };

    std::function<void(const aiNode*, const aiMatrix4x4&)> dfs =
        [&](const aiNode* node, const aiMatrix4x4& parent)
        {
            aiMatrix4x4 global = parent * node->mTransformation;

            for (unsigned mi = 0; mi < node->mNumMeshes; ++mi)
            {
                unsigned meshIndex = node->mMeshes[mi];

                // ← ここが m_MeshWorld に値を詰めている本体
                //    ここの ToSM が最新定義になっているか必ず確認！
                m_MeshWorld[meshIndex] = ToSM(global);
            }

            for (unsigned i = 0; i < node->mNumChildren; ++i)
                dfs(node->mChildren[i], global);
        };

    dfs(m_pScene->mRootNode, aiMatrix4x4());
}



//void CStaticMesh::Load(std::string filename, std::string texturedirectory)
//{
//
//	 //m_importer を使ってシーン読み込み
//	m_pScene = m_importer.ReadFile(
//		filename.c_str(),
//		aiProcessPreset_TargetRealtime_MaxQuality |
//		aiProcess_ConvertToLeftHanded |
//		aiProcess_PopulateArmatureData);
//
//	assert(m_pScene);
//
//	std::vector<GM31::GE::myAssimp::SUBSET> subsets{};							// サブセット情報
//	std::vector<std::vector<GM31::GE::myAssimp::VERTEX>> vertices{};			// 頂点データ（メッシュ単位）
//	std::vector<std::vector<unsigned int>> indices{};					// インデックスデータ（メッシュ単位）
//	std::vector<GM31::GE::myAssimp::MATERIAL> materials{};						// マテリアル
//	std::vector<std::unique_ptr<CTexture>> embededtextures{};			// 内蔵テクスチャ群
//
//	 //assimpを使用してモデルデータを取得
//	GM31::GE::myAssimp::GetModelData(filename, texturedirectory);
//
//	subsets = GM31::GE::myAssimp::GetSubsets();								// サブセット情報取得
//	vertices = GM31::GE::myAssimp::GetVertices();								// 頂点データ（メッシュ単位）
//	indices = GM31::GE::myAssimp::GetIndices();								// インデックスデータ（メッシュ単位）
//	materials = GM31::GE::myAssimp::GetMaterials();							// マテリアル情報取得
//
//	m_diffusetextures = GM31::GE::myAssimp::GetDiffuseTextures();				// ｄｉｆｆｕｓｅテクスチャ情報取得	
//
//	 //頂点データ作成
//	int meshidx = 0;
//
//	for (const auto& mv : vertices)
//	{
//		for (auto& v : mv)
//		{
//			VERTEX_3D vertex{};
//			vertex.Position = DirectX::SimpleMath::Vector3(v.pos.x, v.pos.y, v.pos.z);
//			vertex.Normal = DirectX::SimpleMath::Vector3(v.normal.x, v.normal.y, v.normal.z);
//			vertex.TexCoord = DirectX::SimpleMath::Vector2(v.texcoord.x, v.texcoord.y);
//			vertex.Diffuse = DirectX::SimpleMath::Color(v.color.r, v.color.g, v.color.b, v.color.a);
//
//			vertex.bonecnt = v.bonecnt;
//			for (int i = 0; i < 4; i++)
//			{
//				vertex.BoneIndex[i] = 0;
//				vertex.BoneWeight[i] = 0.0f;
//				vertex.BoneName[i] = "";
//			}
//
//			for (int i = 0; i < v.bonecnt; i++)
//			{
//				vertex.BoneIndex[i] = v.BoneIndex[i];
//				vertex.BoneWeight[i] = v.BoneWeight[i];
//				vertex.BoneName[i] = v.BoneName[i];
//			}
//
//			m_vertices.emplace_back(vertex);
//		}
//	}
//
//	 //インデックスデータ作成
//	for (const auto& mi : indices)
//	{
//		for (auto& index : mi)
//		{
//			m_indices.emplace_back(index);
//		}
//	}
//
//	 //サブセットデータ作成
//	for (const auto& sub : subsets)
//	{
//		SUBSET subset{};
//		subset.VertexBase = sub.VertexBase;
//		subset.VertexNum = sub.VertexNum;
//		subset.IndexBase = sub.IndexBase;
//		subset.IndexNum = sub.IndexNum;
//		subset.MtrlName = sub.mtrlname;
//		subset.MaterialIdx = sub.materialindex;					//	マテリアル配列のインデックス
//		m_subsets.emplace_back(subset);
//	}
//
//	 //マテリアルデータ作成(表示のための)
//	for (const auto& m : materials)
//	{
//		MATERIAL material{};
//		material.Ambient = DirectX::SimpleMath::Color(m.Ambient.r, m.Ambient.g, m.Ambient.b, m.Ambient.a);
//		material.Diffuse = DirectX::SimpleMath::Color(m.Diffuse.r, m.Diffuse.g, m.Diffuse.b, m.Diffuse.a);
//		material.Specular = DirectX::SimpleMath::Color(m.Specular.r, m.Specular.g, m.Specular.b, m.Specular.a);
//		material.Emission = DirectX::SimpleMath::Color(m.Emission.r, m.Emission.g, m.Emission.b, m.Emission.a);
//		material.Shiness = m.Shiness;
//		if (m.diffusetexturename.empty())
//		{
//			material.TextureEnable = FALSE;
//			m_diffusetexturenames.emplace_back("");
//		}
//		else
//		{
//			material.TextureEnable = TRUE;
//			m_diffusetexturenames.emplace_back(m.diffusetexturename);
//		}
//
//		m_materials.emplace_back(material);
//	}
//}

void CStaticMesh::DrawWithBones(const SRT& meshSrt, const Color& meshColor)
{

	//シーンがnullなら描画しない
	if (!m_pScene) return;


	// ボーン辞書を BoneDict 型で取得
	const BoneDict& dict = GM31::GE::myAssimp::GetBoneDictionary();

	static Sphere boneViz(1.05f);
	const Color  boneColor(1, 1, 0, 1);

	DrawBoneSpheresRecursive(
		m_pScene->mRootNode,
		aiMatrix4x4(),  // 単位行列
		meshSrt,
		boneColor,
		boneViz,
		dict);

}

// 再帰ヘルパーは匿名名前空間に置けば外部に見えません
void CStaticMesh::DrawBoneSpheresRecursive(const aiNode* node, const aiMatrix4x4& parentMat, const SRT& meshSrt, const Color& boneColor, Sphere& boneViz, const BoneDict& dict)
{
	// ① このノード名で辞書に登録があればローカル行列を取出し
	aiMatrix4x4 localM;
	auto it = dict.find(node->mName.C_Str());
	if (it != dict.end()) {
		// BONE.AnimationMatrix を使う
		localM = it->second.AnimationMatrix;
	}
	else {
		// 辞書にないノード（非ボーン）はそのままノード変換を使う
		localM = node->mTransformation;
	}

	// ② 親→自分の累積ワールド行列
	aiMatrix4x4 worldAiM = parentMat * localM;

	// ③ 原点を変換してワールド座標を得る
	//    operator* が無ければ a4,b4,c4 を直接読む
	aiVector3D wp(worldAiM.a4, worldAiM.b4, worldAiM.c4);

	// ④ SRT に詰め替えて球を描画
	SRT srt;
	float r = boneViz.GetRadius();
	srt.scale = Vector3(r, r, r);
	srt.rot = Vector3(0, 0, 0);
	srt.pos = meshSrt.pos + Vector3(wp.x, wp.y, wp.z);
	boneViz.Draw(srt, boneColor);

	// ⑤ 子ノードも同様に描画
	for (unsigned i = 0; i < node->mNumChildren; ++i) {
		DrawBoneSpheresRecursive(
			node->mChildren[i],
			worldAiM,
			meshSrt,
			boneColor,
			boneViz,
			dict);
	}

}


	
	//// 3) メッシュ単位のボーン配列をループ
	//for (auto& bones : m_bonesPerMesh)
	//{
	//	GM31::GE::myAssimp::UpdateGlobalMatrices(bones);
	//	for (const auto& b : bones)
	//	{

	//		// ボーンのグローバル行列（親子考慮済み）
	//		aiMatrix4x4 aiM = b.Matrix;

	//		// 4x4 行列に変換
	//		Matrix4x4 boneMtx = ConvertAiToMatrix4x4(aiM);

	//		// スケール行列を掛けて球体サイズを調整
	//		Matrix4x4 scaleM = Matrix4x4::CreateScale(boneViz.Getradius());

	//		// 最終行列 = scale → bone transform → meshSrt のワールド変換
	//		// meshSrt をワールドにそのまま適用したい場合は以下のように
	//		Matrix4x4 worldM = scaleM * boneMtx * meshSrt.GetMatrix();

	//		// 球体を描画
	//		boneViz.Draw(worldM, boneColor);
	//		//boneViz.Draw(meshSrt, boneColor);

	//		//重いから一旦無効化
	//		/*aiVector3D worldPos = b.Matrix * aiVector3D(0, 0, 0);
	//		std::cout << b.bonename << " world pos: "
	//			<< worldPos.x << ", " << worldPos.y << ", " << worldPos.z << std::endl;*/

	//	}
	//}


