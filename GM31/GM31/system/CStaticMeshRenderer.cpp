#include "CStaticMeshRenderer.h"
#include "dx11helper.h"
#include <iostream>

// aiMatrix4x4 → SimpleMath::Matrix 変換ヘルパー
static DirectX::SimpleMath::Matrix ToSM(const aiMatrix4x4& m)
{
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

void DumpMetaData(const aiScene* scene)
{
    if (!scene->mMetaData) {
        std::cout << "メタデータなし\n";
        return;
    }
    aiMetadata* meta = scene->mMetaData;
    std::cout << "=== MetaData (" << meta->mNumProperties << " items) ===\n";

    for (unsigned i = 0; i < meta->mNumProperties; ++i) {
        // キーの取得
        const aiString& keyStr = meta->mKeys[i];
        const char* key = keyStr.C_Str();

        const aiMetadataEntry& entry = meta->mValues[i];
        std::cout << key << " = ";

        switch (entry.mType) {
        case AI_BOOL: {
            bool v = *reinterpret_cast<bool*>(entry.mData);
            std::cout << (v ? "true" : "false");
            break;
        }
        case AI_INT32: {
            int v = *reinterpret_cast<int*>(entry.mData);
            std::cout << v;
            break;
        }
        case AI_UINT64: {
            uint64_t v = *reinterpret_cast<uint64_t*>(entry.mData);
            std::cout << v;
            break;
        }
        case AI_DOUBLE: {
            double v = *reinterpret_cast<double*>(entry.mData);
            std::cout << v;
            break;
        }
        case AI_AISTRING: {
            aiString* s = reinterpret_cast<aiString*>(entry.mData);
            std::cout << (s ? s->C_Str() : "(null)");
            break;
        }
        default:
            std::cout << "(unknown type)";
        }
        std::cout << "\n";
    }
    std::cout << "===============================\n";
}

void PrintMatrix(const aiMatrix4x4& m)
{
    // 行ごとに出力（row-major 表示）
    printf("%8.4f %8.4f %8.4f %8.4f\n", m.a1, m.a2, m.a3, m.a4);
    printf("%8.4f %8.4f %8.4f %8.4f\n", m.b1, m.b2, m.b3, m.b4);
    printf("%8.4f %8.4f %8.4f %8.4f\n", m.c1, m.c2, m.c3, m.c4);
    printf("%8.4f %8.4f %8.4f %8.4f\n", m.d1, m.d2, m.d3, m.d4);
}

void DumpRootTransform(const aiScene* scene)
{
    const aiNode* root = scene->mRootNode;
    printf("=== Root Node Transformation ===\n");
    PrintMatrix(root->mTransformation);
    printf("=================================\n");
}



void CStaticMeshRenderer::Init(CStaticMesh& mesh)
{
    // 1) 頂点・インデックス・サブセット・マテリアル初期化
    CMeshRenderer::Init(mesh);
    m_Subsets = mesh.GetSubsets();
    m_DiffuseTextures = mesh.GetDiffuseTextures();
    auto mats = mesh.GetMaterials();
    for (auto& m : mats)
    {
        auto cm = std::make_unique<CMaterial>();
        cm->Create(m);
        m_Materiales.push_back(std::move(cm));
    }

    // 2) メッシュ毎に計算済みワールド行列をコピー
    m_Worlds = mesh.GetMeshWorldTransforms();

    // サブセット数に合わせて足りなければ Identity で埋める
    if (m_Worlds.size() < m_Subsets.size())
    {
        m_Worlds.resize(
            m_Subsets.size(),
            DirectX::SimpleMath::Matrix::Identity
        );
    }

    // 3) ワールド行列用の定数バッファを作成
    /*ID3D11Device* dev = Renderer::GetDevice();
    CreateConstantBuffer(
        dev,
        sizeof(DirectX::XMFLOAT4X4),
        m_pWorldCB.GetAddressOf()
    );*/

    m_pScene = mesh.GetScene();                       // Assimp シーンを取得
    m_BoneDict = GM31::GE::myAssimp::GetBoneDictionary();
    m_BoneViz = std::make_unique<Sphere>(0.5f);

    if (m_pScene == NULL)
    {
        return;
    }

    //Sceneの中身出力
    DumpMetaData(m_pScene);
    DumpRootTransform(m_pScene);

    //// ボーン用CB作成 (b3 を想定)
    ID3D11Device* dev = Renderer::GetDevice();
    CreateConstantBuffer(dev, sizeof(DirectX::XMFLOAT4X4), m_pBoneWorldCB.GetAddressOf());
}


void CStaticMeshRenderer::Draw()
{
    // インデックスバッファ・頂点バッファをセット
    BeforeDraw();

    // マテリアル数分ループ 
    for (int i = 0; i < m_Subsets.size(); i++)
    {
        // マテリアルをセット(サブセット情報の中にあるマテリアルインデックを使用する)
        m_Materiales[m_Subsets[i].MaterialIdx]->SetGPU();

        if (m_Materiales[m_Subsets[i].MaterialIdx]->isDiffuseTextureEnable())
        {
            m_DiffuseTextures[m_Subsets[i].MaterialIdx]->SetGPU();
        }

        // サブセットの描画
        DrawSubset(
            m_Subsets[i].IndexNum,							// 描画するインデックス数
            m_Subsets[i].IndexBase,							// 最初のインデックスバッファの位置	
            m_Subsets[i].VertexBase);						// 頂点バッファの最初から使用
    }

}

void CStaticMeshRenderer::Draw(const SRT& objectSrt)
{
    std::cout << "それもう使ってないDrawですよ";

}

void CStaticMeshRenderer::DrawWithBones(SRT srt, const Color& boneColor)
{

    //Matrix4x4 world = srt.GetMatrix();

    ////printf("ボーン座標ログ\n");

    //// サブメッシュ数分だけ回して、「SRT × Assimpノード行列」を計算し、
    //for (size_t meshIdx = 0; meshIdx < m_Worlds.size(); ++meshIdx)
    //{
    //    // Assimp で計算済みのサブメッシュワールド
    //    const auto& modelWorld = m_Worlds[meshIdx];

    //    // プレイヤーの移動／回転／拡縮を先に掛ける
    //    Matrix4x4 objWorld = world * modelWorld;

    //    // ここで一度だけルートから骨を描画
    //    DrawBoneRecursive(
    //        m_pScene->mRootNode,    // ノード階層のルート
    //        aiMatrix4x4{},          // 親行列＝Identity
    //        boneColor,              // 好きな色
    //        objWorld,               // 先ほど組んだ最終ワールド
    //        srt                     // 球のスケールに使うだけ
    //    );
    //}

    //printf("ボーン座標ログ終わり\n");

    if (!m_pScene) return;
    auto ctx = Renderer::GetDeviceContext();

    // GPU 側の World は既に SetWorldMatrix() でセット済み
    // ここではボーン描画の「局所行列のみ」を計算して Sphere に投げる
    DrawBoneRecursive(
        m_pScene->mRootNode,
        aiMatrix4x4(),      // 親 Ai 行列 = Identity
        boneColor,
        srt);

}

void CStaticMeshRenderer::DrawBoneRecursive(
    const aiNode* node,
    const aiMatrix4x4& parentAiM,
    const Color& boneColor,
    SRT  srt)
{
    // ① ボーンのローカル行列を取得
    aiMatrix4x4 localAiM = node->mTransformation ;
    aiMatrix4x4 globalAiM = parentAiM * localAiM;

    // ② モデルのワールド行列（SRT → Matrix4x4）
    Matrix4x4 modelWorld = srt.GetMatrix();

    // ③ ボーンのローカル行列を自前の型に変換
    Matrix4x4 boneLocal = ToSM(globalAiM);

    // ④ 最終ワールド行列を計算
    Matrix4x4 boneWorld = modelWorld * boneLocal;

    // ⑤ GPU にセットしてスフィアを描画
    Renderer::SetWorldMatrix(&boneWorld);


    Vector3 rawPos = Vector3(globalAiM.a4, globalAiM.b4, globalAiM.c4);

    auto q = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
        srt.rot.y,   // Yaw   (Y 軸周り)
        srt.rot.x,   // Pitch (X 軸周り)
        srt.rot.z    // Roll  (Z 軸周り)
    );

    // 回転
    Vector3 rotated = Vector3::Transform(rawPos, q);
    Vector3 scaledPos = Vector3(
        rotated.x * srt.scale.x,
        rotated.y * srt.scale.y,
        rotated.z * srt.scale.z
    );
    //座標情報を入れる
    Vector3 worldPos = scaledPos + srt.pos;

    //接続部のログを出す
    //LogBoneWorldPosition("Conect", srt);

    // スフィア自体は単位球＋半径で OK
    // sphere.Draw() 内で SetWorldMatrix()→上書きされないよう、
    // srts は位置・回転ゼロ、拡縮だけ r。
    SRT srts;
    srts.scale = Vector3(m_BoneViz->GetRadius());
    srts.rot = Vector3::Zero;
    //srts.pos = Vector3(globalAiM.a4 + srt.pos.x, globalAiM.b4 + srt.pos.y, globalAiM.c4 + srt.pos.z);
    srts.pos = worldPos;
    m_BoneViz->Draw(srts, boneColor);

    // ⑥ 子ノードへ再帰
    for (UINT i = 0; i < node->mNumChildren; ++i) {
        DrawBoneRecursive(node->mChildren[i], globalAiM, boneColor, srt);
    }

}

void CStaticMeshRenderer::LogBoneWorldPosition(const std::string& targetName, const SRT& srt)
{
    if (!m_pScene) return;
    // Identity を親行列に渡してルートから探索
    FindAndLogBoneRecursive(
        m_pScene->mRootNode,
        aiMatrix4x4(),    // 親 Ai 行列 = Identity
        targetName,
        srt);
}

bool CStaticMeshRenderer::FindAndLogBoneRecursive(const aiNode* node, const aiMatrix4x4& parentAiM,
    const std::string& targetName, const SRT& srt) 
{
    // ● Assimp のローカル行列 → グローバル行列
    aiMatrix4x4 globalAiM = parentAiM * node->mTransformation;

    // ● ボーンのオブジェクト空間位置を取り出し
    Vector3 rawPos{ globalAiM.a4,
                    globalAiM.b4,
                    globalAiM.c4 };

    // ● オブジェクトの回転をクォータニオン化
    auto q = Quaternion::CreateFromYawPitchRoll(
        srt.rot.y,    // Yaw
        srt.rot.x,    // Pitch
        srt.rot.z     // Roll
    );

    // ● 回転 → スケール → 平行移動 の順でワールド座標を算出
    Vector3 rotated = Vector3::Transform(rawPos, q);
    Vector3 scaledPos = {
        rotated.x * srt.scale.x,
        rotated.y * srt.scale.y,
        rotated.z * srt.scale.z
    };
    Vector3 worldPos = scaledPos + srt.pos;

    // ● ノード名が一致したらログ出力して true を返す
    if (node->mName.C_Str() == targetName)
    {
        std::cout
            << "Bone[" << node->mName.C_Str() << "] "
            << "WorldPos = "
            << worldPos.x << ", "
            << worldPos.y << ", "
            << worldPos.z << "\n";
        return true;
    }

    // ● 子ノードを探索。見つかれば true を戻して打ち切り
    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        if (FindAndLogBoneRecursive(
            node->mChildren[i],
            globalAiM,
            targetName,
            srt))
        {
            return true;
        }
    }

    return false;
}