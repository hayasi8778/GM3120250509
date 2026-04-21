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



// 共通ユーティリティ（既存ファイルの先頭付近に追加）
void DumpMatrix4x4(const Matrix4x4& m, const char* tag)
{
    printf("%s (Matrix4x4) =\n", tag);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", m._11, m._12, m._13, m._14);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", m._21, m._22, m._23, m._24);
    printf(" %8.4f %8.4f %8.4f %8.4f\n", m._31, m._32, m._33, m._34);
    printf(" %8.4f %8.4f %8.4f %8.4f\n\n", m._41, m._42, m._43, m._44);
}

void DumpAiMatrixShort(const aiMatrix4x4& a, const char* tag)
{
    printf("%s (aiMatrix4x4) transl = %8.4f, %8.4f, %8.4f\n",
        tag, a.a4, a.b4, a.c4);
}

void DumpMatrix4x4Short(const Matrix4x4& m, const char* tag)
{
    printf("%s (Matrix4x4) transl = %8.4f, %8.4f, %8.4f\n",
        tag, m._41, m._42, m._43);
}

void PrintDiffTransl(const Matrix4x4& a, const Matrix4x4& b, const char* tagA, const char* tagB)
{
    float dx = a._41 - b._41;
    float dy = a._42 - b._42;
    float dz = a._43 - b._43;
    printf("DIFF transl %s - %s = %8.6f, %8.6f, %8.6f\n", tagA, tagB, dx, dy, dz);
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

//SimpleMathのToMSの変換ヘルパー
Matrix4x4  CStaticMesh::ToSM_Matrix4x4(const aiMatrix4x4& m)
{
    Matrix4x4 r;

    r._11 = m.a1; r._12 = m.b1; r._13 = m.c1; r._14 = m.d1;
    r._21 = m.a2; r._22 = m.b2; r._23 = m.c2; r._24 = m.d2;
    r._31 = m.a3; r._32 = m.b3; r._33 = m.c3; r._34 = m.d3;
    r._41 = m.a4; r._42 = m.b4; r._43 = m.c4; r._44 = m.d4;

    return r;
}


void CStaticMesh::Load(std::string filename, std::string texturedirectory)
{

    std::string base = GetBaseName(filename);
    std::string bin = base + ".bin";

    if (FileExists(bin)) {
        LoadFromBinary(bin);
    }
    else {
        LoadWithAssimp(filename, texturedirectory);
        SaveToBinary(bin);
    }

   
    return;
    
}

//バイナリを読み込まず、Assimpでモデルを読み込む方式
// (現状バイナリでのボーン情報の取り込みが未完成なのでAssimpのみの読み込みを追加する)
void CStaticMesh::LoadToAssimp(std::string filename, std::string texturedirectory)
{
    std::string base = GetBaseName(filename);
    std::string bin = base + ".bin";
    
    //同名のバイナリを複数保存しないためにバイナリ化は最初の一回だけ行う
    if (FileExists(bin)) {
        LoadWithAssimp(filename, texturedirectory);
    }
    else {
        LoadWithAssimp(filename, texturedirectory);
        SaveToBinary(bin);
    }

}




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

void CStaticMesh::SaveToBinary(const std::string& binfile)
{
    FILE* fp;
    fopen_s(&fp, binfile.c_str(), "wb");

    MeshBinHeader header;
    header.vertexCount = m_vertices.size();
    header.indexCount = m_indices.size();
    header.subsetCount = m_subsets.size();
    header.materialCount = m_materials.size();
    header.meshWorldCount = m_MeshWorld.size();

    fwrite(&header, sizeof(header), 1, fp);

    //fwrite(m_vertices.data(), sizeof(VERTEX_3D), m_vertices.size(), fp);
    // 頂点数
    uint32_t vertexCount = static_cast<uint32_t>(m_vertices.size());
    fwrite(&vertexCount, sizeof(uint32_t), 1, fp);

    for (auto& v : m_vertices)
    {
        // POD 部分だけそのまま書く
        fwrite(&v.Position, sizeof(Vector3), 1, fp);
        fwrite(&v.Normal, sizeof(Vector3), 1, fp);
        fwrite(&v.Diffuse, sizeof(Color), 1, fp);
        fwrite(&v.TexCoord, sizeof(Vector2), 1, fp);
        fwrite(&v.BoneIndex, sizeof(int), 4, fp);
        fwrite(&v.BoneWeight, sizeof(float), 4, fp);

        // bonecnt
        fwrite(&v.bonecnt, sizeof(int), 1, fp);

        // BoneName は bonecnt 分だけ長さ＋文字列で書く
        for (int i = 0; i < v.bonecnt; ++i)
        {
            uint32_t len = static_cast<uint32_t>(v.BoneName[i].size());
            fwrite(&len, sizeof(uint32_t), 1, fp);
            fwrite(v.BoneName[i].c_str(), 1, len, fp);
        }
    }
    fwrite(m_indices.data(), sizeof(uint32_t), m_indices.size(), fp);
    //fwrite(m_subsets.data(), sizeof(SUBSET), m_subsets.size(), fp);
     //SUBSETの保存
    // --- SUBSET の保存（手動シリアライズ） ---
    uint32_t subsetCount = m_subsets.size();
    fwrite(&subsetCount, sizeof(uint32_t), 1, fp);

    for (auto& s : m_subsets)
    {
        fwrite(&s.VertexBase, sizeof(uint32_t), 1, fp);
        fwrite(&s.VertexNum, sizeof(uint32_t), 1, fp);
        fwrite(&s.IndexBase, sizeof(uint32_t), 1, fp);
        fwrite(&s.IndexNum, sizeof(uint32_t), 1, fp);
        fwrite(&s.MaterialIdx, sizeof(int), 1, fp);

        // 文字列は長さ＋中身
        uint32_t len = s.MtrlName.size();
        fwrite(&len, sizeof(uint32_t), 1, fp);
        fwrite(s.MtrlName.c_str(), 1, len, fp);
    }
    fwrite(m_materials.data(), sizeof(MATERIAL), m_materials.size(), fp);
    fwrite(m_MeshWorld.data(), sizeof(Matrix4x4), m_MeshWorld.size(), fp);

   

    // テクスチャ名は可変長なので文字列長＋文字列で保存
    uint32_t texCount = m_diffusetexturenames.size();
    fwrite(&texCount, sizeof(uint32_t), 1, fp);
    for (auto& name : m_diffusetexturenames) {
        uint32_t len = name.size();
        fwrite(&len, sizeof(uint32_t), 1, fp);
        fwrite(name.c_str(), 1, len, fp);
    }

    //// ボーン辞書を書き出し
    //uint32_t boneCount = static_cast<uint32_t>(m_BoneWorldMap.size());
    //fwrite(&boneCount, sizeof(uint32_t), 1, fp);

    //for (auto& kv : m_BoneWorldMap)
    //{
    //    const std::string& name = kv.first;
    //    const Matrix4x4& mat = kv.second;

    //    uint32_t len = static_cast<uint32_t>(name.size());
    //    fwrite(&len, sizeof(uint32_t), 1, fp);
    //    fwrite(name.c_str(), 1, len, fp);

    //    fwrite(&mat, sizeof(Matrix4x4), 1, fp);
    //}

    // ボーン辞書を書き出し（階層付き）
    uint32_t boneCount = m_bones.size();
    fwrite(&boneCount, sizeof(uint32_t), 1, fp);

    for (auto& b : m_bones)
    {
        // bonename
        uint32_t len = b.bonename.size();
        fwrite(&len, sizeof(uint32_t), 1, fp);
        fwrite(b.bonename.c_str(), 1, len, fp);

        // parentname
        len = b.parentname.size();
        fwrite(&len, sizeof(uint32_t), 1, fp);
        fwrite(b.parentname.c_str(), 1, len, fp);

        // AnimationMatrix（ローカル行列）
        fwrite(&b.AnimationMatrix, sizeof(Matrix4x4), 1, fp);
    }

    fclose(fp);
}


void CStaticMesh::LoadFromBinary(const std::string& binfile)
{
    FILE* fp;
    fopen_s(&fp, binfile.c_str(), "rb");

    MeshBinHeader header;
    fread(&header, sizeof(header), 1, fp);

    //m_vertices.resize(header.vertexCount);
    m_indices.resize(header.indexCount);
    //m_subsets.resize(header.subsetCount);
    m_materials.resize(header.materialCount);
    m_MeshWorld.resize(header.meshWorldCount);

    //fread(m_vertices.data(), sizeof(VERTEX_3D), header.vertexCount, fp);
    // 頂点数を読む
    uint32_t vertexCount = 0;
    fread(&vertexCount, sizeof(uint32_t), 1, fp);

    m_vertices.clear();
    m_vertices.resize(vertexCount);

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        VERTEX_3D v{};

        fread(&v.Position, sizeof(Vector3), 1, fp);
        fread(&v.Normal, sizeof(Vector3), 1, fp);
        fread(&v.Diffuse, sizeof(Color), 1, fp);
        fread(&v.TexCoord, sizeof(Vector2), 1, fp);
        fread(&v.BoneIndex, sizeof(int), 4, fp);
        fread(&v.BoneWeight, sizeof(float), 4, fp);

        fread(&v.bonecnt, sizeof(int), 1, fp);

        for (int j = 0; j < v.bonecnt; ++j)
        {
            uint32_t len = 0;
            fread(&len, sizeof(uint32_t), 1, fp);

            v.BoneName[j].resize(len);
            fread(&v.BoneName[j][0], 1, len, fp);
        }

        m_vertices[i] = std::move(v);
    }
    fread(m_indices.data(), sizeof(uint32_t), header.indexCount, fp);
    //fread(m_subsets.data(), sizeof(SUBSET), header.subsetCount, fp);
    // --- SUBSET の読み込み ---
    uint32_t subsetCount = 0;
    fread(&subsetCount, sizeof(uint32_t), 1, fp);

    m_subsets.resize(subsetCount);

    for (uint32_t i = 0; i < subsetCount; i++)
    {
        SUBSET s;

        fread(&s.VertexBase, sizeof(uint32_t), 1, fp);
        fread(&s.VertexNum, sizeof(uint32_t), 1, fp);
        fread(&s.IndexBase, sizeof(uint32_t), 1, fp);
        fread(&s.IndexNum, sizeof(uint32_t), 1, fp);
        fread(&s.MaterialIdx, sizeof(int), 1, fp);

        uint32_t len = 0;
        fread(&len, sizeof(uint32_t), 1, fp);

        s.MtrlName.resize(len);
        fread(&s.MtrlName[0], 1, len, fp);

        m_subsets[i] = std::move(s);
    }
    fread(m_materials.data(), sizeof(MATERIAL), header.materialCount, fp);
    fread(m_MeshWorld.data(), sizeof(Matrix4x4), header.meshWorldCount, fp);


    // --- テクスチャ名読み込み ---
    uint32_t texCount = 0;
    fread(&texCount, sizeof(uint32_t), 1, fp);

    m_diffusetexturenames.resize(texCount);

    for (uint32_t i = 0; i < texCount; i++)
    {
        uint32_t len = 0;
        fread(&len, sizeof(uint32_t), 1, fp);

        m_diffusetexturenames[i].resize(len);
        fread(&m_diffusetexturenames[i][0], 1, len, fp);
    }

    //// --- ボーン辞書の読み込み ---
    //uint32_t boneCount = 0;
    //fread(&boneCount, sizeof(uint32_t), 1, fp);

    //m_BoneWorldMap.clear();

    //for (uint32_t i = 0; i < boneCount; ++i)
    //{
    //    uint32_t len = 0;
    //    fread(&len, sizeof(uint32_t), 1, fp);

    //    std::string name;
    //    name.resize(len);
    //    fread(&name[0], 1, len, fp);

    //    Matrix4x4 mat{};
    //    fread(&mat, sizeof(Matrix4x4), 1, fp);

    //    m_BoneWorldMap[name] = mat;
    //}

    // --- ボーン辞書の読み込み（階層付き） ---
    uint32_t boneCount = 0;
    fread(&boneCount, sizeof(uint32_t), 1, fp);

    m_bones.resize(boneCount);

    for (uint32_t i = 0; i < boneCount; ++i)
    {
        uint32_t len = 0;

        // bonename
        fread(&len, sizeof(uint32_t), 1, fp);
        m_bones[i].bonename.resize(len);
        fread(&m_bones[i].bonename[0], 1, len, fp);

        // parentname
        fread(&len, sizeof(uint32_t), 1, fp);
        m_bones[i].parentname.resize(len);
        fread(&m_bones[i].parentname[0], 1, len, fp);

        // AnimationMatrix（ローカル）
        fread(&m_bones[i].AnimationMatrix, sizeof(Matrix4x4), 1, fp);
    }

    //ワールド行列の再構築
    std::unordered_map<std::string, uint32_t> nameToIndex;
    for (uint32_t i = 0; i < boneCount; ++i)
        nameToIndex[m_bones[i].bonename] = i;

    std::vector<bool> done(boneCount, false);

    std::function<void(uint32_t)> buildWorld = [&](uint32_t idx)
        {
            if (done[idx]) return;

            auto& b = m_bones[idx];

            if (b.parentname.empty())
            {
                b.Matrix = b.AnimationMatrix;
            }
            else
            {
                uint32_t pidx = nameToIndex[b.parentname];
                buildWorld(pidx);
                b.Matrix = m_bones[pidx].Matrix * b.AnimationMatrix;
            }

            done[idx] = true;
        };

    for (uint32_t i = 0; i < boneCount; ++i)
        buildWorld(i);

    // 最後に m_BoneWorldMap を作る
    m_BoneWorldMap.clear();
    for (auto& b : m_bones) {
        m_BoneWorldMap[b.bonename] = b.Matrix;
    }
       

    //テクスチャのロード前にバイナリの読み込み終える
    fclose(fp);

    // --- テクスチャ実体のロード（バイナリとは無関係） ---
    m_diffusetextures.clear();
    m_diffusetextures.resize(texCount);

    for (uint32_t i = 0; i < texCount; i++)
    {
        const std::string& path = m_diffusetexturenames[i];

        if (!path.empty())
        {
            auto tex = std::make_unique<CTexture>();

            if (tex->Load(path))
            {
                m_diffusetextures[i] = std::move(tex);
            }
            else
            {
                std::cout << "Texture load failed: " + path + "\n";
            }
        }
    }

    ////ボーンの最終確認
    //printf("bones = %u, BoneWorldMap = %zu\n",
    //    boneCount,
    //    m_BoneWorldMap.size());

    //printf("=== BoneWorldMap keys ===\n");
    //for (auto& kv : m_BoneWorldMap)
    //{
    //    printf("%s\n", kv.first.c_str());
    //}
    //printf("=========================\n");

}

bool CStaticMesh::FileExists(const std::string& path)
{
    FILE* fp;
    fopen_s(&fp, path.c_str(), "rb");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}

std::string CStaticMesh::GetBaseName(const std::string& path)
{
    //.から前の部分(拡張子以外の部分)を抽出して返す
    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos) return path;
    return path.substr(0, pos);
}

//Asimp側のロード(バイナリ実装前のものをそのまま流用)
void CStaticMesh::LoadWithAssimp(std::string filename, std::string texturedirectory)
{
    //複数回読み込むと重くなるらしいから毎回クリアする(チャットAI曰く)
    m_vertices.clear();
    m_indices.clear();
    m_subsets.clear();
    m_materials.clear();
    m_diffusetexturenames.clear();
    m_MeshWorld.clear();
    m_diffusetextures.clear();

    //バイナリの読み込み出来ないなら普通に読み込むようにしたいから一応残しておく
    m_importer.SetPropertyInteger(AI_CONFIG_IMPORT_COLLADA_IGNORE_UP_DIRECTION, 0);

    // 1) assimp でシーン読み込み
    //m_pScene = m_importer.ReadFile(
    //    filename.c_str(),
    //    aiProcessPreset_TargetRealtime_MaxQuality |
    //    aiProcess_ConvertToLeftHanded |
    //    aiProcess_PopulateArmatureData
    //);
    
    //バイナリ読み込みの方のモデル読み込み
    m_pScene = m_importer.ReadFile(
        filename.c_str(),
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        //aiProcess_JoinIdenticalVertices |
        //aiProcess_LimitBoneWeights |
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
        //m_diffusetexturenames.emplace_back(m.diffusetexturename);
        //完全パスを保存するようにする
        m_diffusetexturenames.emplace_back(texturedirectory + "/" + m.diffusetexturename);
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

    //ボーンデータを保存
    m_bones.clear();


    std::function<void(const aiNode*, const aiNode*, const aiMatrix4x4&)> dfsBone =
        [&](const aiNode* node, const aiNode* parentNode, const aiMatrix4x4& parentGlobal)
        {
            // 親のワールド行列 × 自分のローカル行列
            aiMatrix4x4 global = parentGlobal * node->mTransformation;

            BONE b;
            b.bonename = node->mName.C_Str();
            b.parentname = parentNode ? parentNode->mName.C_Str() : "";

            // ローカル行列（AnimationMatrix）
            //b.AnimationMatrix = ToSM(node->mTransformation);
            b.AnimationMatrix = ToSM_Matrix4x4(node->mTransformation);

            // ワールド行列（Matrix）※保存はしないが、Assimp直読み時に使う
            //b.Matrix = ToSM(global);
            b.Matrix = ToSM_Matrix4x4(global);

            m_bones.push_back(std::move(b));

            // 子ノードへ
            for (unsigned i = 0; i < node->mNumChildren; ++i)
                dfsBone(node->mChildren[i], node, global);
        };

    // ルートは親なし、親ワールド行列は単位行列
    dfsBone(m_pScene->mRootNode, nullptr, aiMatrix4x4());

    //printf("[Assimp] BoneWorldMap size = %zu\n", m_BoneWorldMap.size());
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
