#include    "commontypes.h"
#include	"CStaticMesh.h"
#include	"C3DShape.h"
#include	<iostream>

void CStaticMesh::Load(std::string filename, std::string texturedirectory)
{
	std::vector<GM31::GE::myAssimp::SUBSET> subsets{};					// サブセット情報
	std::vector<std::vector<GM31::GE::myAssimp::VERTEX>> vertices{};	// 頂点データ（メッシュ単位）
	std::vector<std::vector<unsigned int>> indices{};					// インデックスデータ（メッシュ単位）
	std::vector<GM31::GE::myAssimp::MATERIAL> materials{};				// マテリアル
	std::vector<std::unique_ptr<CTexture>> embededtextures{};			// 内蔵テクスチャ群

	// assimpを使用してモデルデータを取得
	GM31::GE::myAssimp::GetModelData(filename, texturedirectory);

	subsets = GM31::GE::myAssimp::GetSubsets();							// サブセット情報取得
	vertices = GM31::GE::myAssimp::GetVertices();						// 頂点データ（メッシュ単位）
	indices = GM31::GE::myAssimp::GetIndices();							// インデックスデータ（メッシュ単位）
	materials = GM31::GE::myAssimp::GetMaterials();						// マテリアル情報取得

	//ボーン情報取り出す
	auto boneDict = GM31::GE::myAssimp::GetBoneDictionary();
	// boneDict は key=boneName, value=BONE 構造体

	 //g_BonesPerMeshes をそのまま public にしてあるので…
	auto& bonesPerMesh = GM31::GE::myAssimp::GetBonesPerMeshes;

	


	// もし getter が欲しければ、ヘッダに
	// std::vector<std::vector<BONE>>& GetBonesPerMeshes();
	// を追加して GetBonesPerMeshes() を返すように実装してください



	m_diffusetextures = GM31::GE::myAssimp::GetDiffuseTextures();		// ｄｉｆｆｕｓｅテクスチャ情報取得	

	// 頂点データ作成
	int meshidx = 0;

	for (const auto& mv : vertices)
	{
		for (auto& v : mv)
		{
			VERTEX_3D vertex{};
			vertex.Position = Vector3(v.pos.x, v.pos.y, v.pos.z);
			vertex.Normal = Vector3(v.normal.x, v.normal.y, v.normal.z);
			vertex.TexCoord = Vector2(v.texcoord.x, v.texcoord.y);
			vertex.Diffuse = Color(v.color.r, v.color.g, v.color.b, v.color.a);

			vertex.bonecnt = v.bonecnt;
			for (int i = 0; i < 4; i++)
			{
				vertex.BoneIndex[i] = 0;
				vertex.BoneWeight[i] = 0.0f;
				vertex.BoneName[i] = "";
			}

			for (int i = 0; i < v.bonecnt; i++) 
			{
				vertex.BoneIndex[i] = v.BoneIndex[i];
				vertex.BoneWeight[i] = v.BoneWeight[i];
				vertex.BoneName[i] = v.BoneName[i];
			}

			m_vertices.emplace_back(vertex);
		}
	}

	// インデックスデータ作成
	for (const auto& mi : indices)
	{
		for (auto& index : mi)
		{
			m_indices.emplace_back(index);
		}
	}

	// サブセットデータ作成
	for (const auto& sub : subsets)
	{
		SUBSET subset{};
		subset.VertexBase = sub.VertexBase;
		subset.VertexNum = sub.VertexNum;
		subset.IndexBase = sub.IndexBase;
		subset.IndexNum = sub.IndexNum;
		subset.MtrlName = sub.mtrlname;
		subset.MaterialIdx = sub.materialindex;					//	マテリアル配列のインデックス
		m_subsets.emplace_back(subset);
	}

	// マテリアルデータ作成(表示のための)
	for (const auto& m : materials)
	{
		MATERIAL material{};
		material.Ambient = Color(m.Ambient.r, m.Ambient.g, m.Ambient.b, m.Ambient.a);
		material.Diffuse = Color(m.Diffuse.r, m.Diffuse.g, m.Diffuse.b, m.Diffuse.a);
		material.Specular = Color(m.Specular.r, m.Specular.g, m.Specular.b, m.Specular.a);
		material.Emission = Color(m.Emission.r, m.Emission.g, m.Emission.b, m.Emission.a);
		material.Shiness = m.Shiness;
		if (m.diffusetexturename.empty())
		{
			material.TextureEnable = FALSE;
			m_diffusetexturenames.emplace_back("");
		}
		else
		{
			material.TextureEnable = TRUE;
			m_diffusetexturenames.emplace_back(m.diffusetexturename);
		}

		m_materials.emplace_back(material);
	}

	// Load の最後に
	m_bonesPerMesh = GM31::GE::myAssimp::GetBonesPerMeshes();
}

void CStaticMesh::DrawWithBones(const SRT& meshSrt, const Color& meshColor)
{

	// 2) ボーン可視化用の球体インスタンスを用意
	static Sphere boneViz(1.05f);      // 半径 0.05f
	const Color boneColor(1, 1, 0, 1);    // 黄色

	for (auto& bones : m_bonesPerMesh)
	{
		// ボーン行列を階層合成
		GM31::GE::myAssimp::UpdateGlobalMatrices(bones);

		for (auto& b : bones)
		{
			// ① ボーンのワールド位置を aiVector3D で取得
			aiVector3D localOrigin(0, 0, 0);
			aiVector3D wp = b.Matrix * localOrigin;

			// ② SRT を組み立て
			SRT srt;

			// 球体の大きさを scale にセット
			srt.scale = Vector3(boneViz.GetRadius(),
				boneViz.GetRadius(),
				boneViz.GetRadius());

			// 今回は回転なし
			srt.rot = Vector3(0, 0, 0);

			// メッシュのワールド変換も考慮するなら
			// ai→自前Vector3 への変換
			Vector3 bonePos(wp.x, wp.y, wp.z);
			// meshSrt も乗算したい場合は行列で計算するか、
			// meshSrt.pos + bonePos のように加算します
			srt.pos = meshSrt.pos + bonePos;

			// ③ SRT版 Draw を呼ぶ
			boneViz.Draw(srt, boneColor);
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

}
