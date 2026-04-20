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

// SimpleMath::Matrix →  aiMatrix4x4変換ヘルパーも作る
aiMatrix4x4 ToAi(const Matrix4x4& m)
{
    return aiMatrix4x4(
        m._11, m._12, m._13, m._14,
        m._21, m._22, m._23, m._24,
        m._31, m._32, m._33, m._34,
        m._41, m._42, m._43, m._44
    );
}

SRT MatrixToSRT(const Matrix4x4& m)
{
    SRT srt;

    // --- 平行移動 ---
    srt.pos = { m._41, m._42, m._43 };

    // --- スケール ---
    Vector3 axisX(m._11, m._12, m._13);
    Vector3 axisY(m._21, m._22, m._23);
    Vector3 axisZ(m._31, m._32, m._33);

    srt.scale.x = axisX.Length();
    srt.scale.y = axisY.Length();
    srt.scale.z = axisZ.Length();

    // --- 回転行列（スケール除去後） ---
    if (srt.scale.x != 0) axisX /= srt.scale.x;
    if (srt.scale.y != 0) axisY /= srt.scale.y;
    if (srt.scale.z != 0) axisZ /= srt.scale.z;

    Matrix4x4 rotMat;
    rotMat._11 = axisX.x; rotMat._12 = axisX.y; rotMat._13 = axisX.z; rotMat._14 = 0;
    rotMat._21 = axisY.x; rotMat._22 = axisY.y; rotMat._23 = axisY.z; rotMat._24 = 0;
    rotMat._31 = axisZ.x; rotMat._32 = axisZ.y; rotMat._33 = axisZ.z; rotMat._34 = 0;
    rotMat._41 = 0;       rotMat._42 = 0;       rotMat._43 = 0;       rotMat._44 = 1;

    // --- 回転行列 → クォータニオン ---
    Quaternion q = Quaternion::CreateFromRotationMatrix(rotMat);

    // --- クォータニオン → オイラー角（YawPitchRoll） ---
    q.Normalize();
    // --- クォータニオン → オイラー角（左手系） ---
        // rot.x = Pitch, rot.y = Yaw, rot.z = Roll
    float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    float pitch = std::atan2(sinr_cosp, cosr_cosp);

    float sinp = 2.0f * (q.w * q.y - q.z * q.x);
    float yaw;
    if (std::abs(sinp) >= 1.0f)
        yaw = std::copysign(DirectX::XM_PIDIV2, sinp); // ±90°
    else
        yaw = std::asin(sinp);

    float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    float roll = std::atan2(siny_cosp, cosy_cosp);


    // rot は (pitch, yaw, roll) ではなく (x=Pitch, y=Yaw, z=Roll) で格納
    srt.rot = { pitch, yaw, roll };

    return srt;
}


// ① GetGlobalAiMatrix(): aiNode 階層を辿って完全な world 行列を再現
aiMatrix4x4 GetGlobalAiMatrix(const aiNode* node)
{
    aiMatrix4x4 m = node->mTransformation;
    while (node->mParent)
    {
        node = node->mParent;
        m = node->mTransformation * m;
    }
    return m;
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
    
    ("%8.4f %8.4f %8.4f %8.4f\n", m.a1, m.a2, m.a3, m.a4);
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

//Weightも中身違うのでコンバーターで渡す
GM31::GE::myAssimp::WEIGHT ConvertWeight(const WEIGHT& src, const std::string& boneName)
{
    GM31::GE::myAssimp::WEIGHT dst;

    dst.bonename = boneName;     // ← CStaticMesh 側には無いので BONE から渡す
    dst.meshname = "";           // 必要なら埋める
    dst.weight = src.weight;
    dst.vertexindex = src.vertexindex;

    return dst;
}

GM31::GE::myAssimp::BONE ConvertBone(const BONE& src)
{
    GM31::GE::myAssimp::BONE dst;

    dst.bonename = src.bonename;
    dst.meshname = src.meshname;
    dst.armaturename = src.armaturename;

    dst.Matrix = ToAi(src.Matrix);
    dst.AnimationMatrix = ToAi(src.AnimationMatrix);
    dst.OffsetMatrix = ToAi(src.OffsetMatrix);

    dst.idx = src.idx;

    //weights の変換
    dst.weights.clear();
    dst.weights.reserve(src.weights.size());

    for (auto& w : src.weights)
    {
        dst.weights.push_back(ConvertWeight(w, src.bonename));
    }

    return dst;
}

void CStaticMeshRenderer::Init(CStaticMesh& mesh)
{
    //meshを後から参照できるように保存する
    m_pMesh = &mesh;


    // 1) 頂点・インデックス・サブセット・マテリアル初期化
    CMeshRenderer::Init(mesh);
    m_Subsets = mesh.GetSubsets();

    auto& srcTex = mesh.GetDiffuseTextures_move();

    m_DiffuseTextures.clear();
    m_DiffuseTextures.resize(m_Subsets.size());

    // srcTex の中身をそのまま移す（move）
    for (size_t i = 0; i < srcTex.size() && i < m_DiffuseTextures.size(); ++i)
    {
        if (srcTex[i])
        {
            // 所有権を移す（共有したいなら clone 方式に変更）
            m_DiffuseTextures[i] = std::move(const_cast<std::unique_ptr<CTexture>&>(srcTex[i]));
        }
    }
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


    if (m_pScene == NULL)
    {
        m_BoneDict.clear();

        auto& bones = mesh.GetBones();

        for (auto& b : bones)
        {
            m_BoneDict[b.bonename] = ConvertBone(b);
        }

        m_BoneViz = std::make_unique<Sphere>(0.5f);

        ID3D11Device* dev = Renderer::GetDevice();
        CreateConstantBuffer(dev, sizeof(DirectX::XMFLOAT4X4), m_pBoneWorldCB.GetAddressOf());

        return;
    }

    m_BoneDict = GM31::GE::myAssimp::GetBoneDictionary();
    m_BoneViz = std::make_unique<Sphere>(0.5f);

    //Sceneの中身出力
    //DumpMetaData(m_pScene);
    //DumpRootTransform(m_pScene);

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
        const int matIdx = m_Subsets[i].MaterialIdx;

        // 安全確認
        if (matIdx < 0 || matIdx >= m_Materiales.size()) {
            std::cout << "matIdx が m_Materiales 範囲外\n" << "endl";
            continue;
        }
        if (matIdx >= m_DiffuseTextures.size()) {
            std::cout << "matIdx が m_DiffuseTextures 範囲外\n" << "endl";
            continue;
        }


        // マテリアルをセット(サブセット情報の中にあるマテリアルインデックを使用する)
        m_Materiales[m_Subsets[i].MaterialIdx]->SetGPU();

        if (m_Materiales[m_Subsets[i].MaterialIdx]->isDiffuseTextureEnable())
        {
            if (!m_DiffuseTextures[matIdx]) {
                std::cout << "DiffuseTexture ポインタが nullptr\n";
            }
            else {
                // ここで SRV が null かどうかを CTexture 側でログ出すようにしてもいい
                m_DiffuseTextures[matIdx]->SetGPU();
            }
            //m_DiffuseTextures[m_Subsets[i].MaterialIdx]->SetGPU();
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
    aiMatrix4x4 localAiM = node->mTransformation;
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

Vector3 CStaticMeshRenderer::LogBoneWorldPosition(const std::string& targetName, const SRT& srt)
{
    if (m_pScene)
    {
        //return Vector3::Zero;  // シーン未設定時は(0,0,0)返却

    Vector3 resultPos{};
    bool found = FindAndLogBoneRecursive(
        m_pScene->mRootNode,
        aiMatrix4x4(),
        targetName,
        srt,
        resultPos);

        //見つかったら値を返す
        if (found)
        {
            // assert(false && "Bone not found: " + targetName);
            return resultPos;
        }

    }

    //これがバイナリならメッシュからポジションを拾う
    if (m_pMesh)
    {
        const auto& boneMap = m_pMesh->GetBoneWorldMap();
        auto it = boneMap.find(targetName);
        if (it != boneMap.end())
        {
            // 行列から平行移動を取り出し
            const Matrix4x4& m = it->second;
            Vector3 local{ m._41, m._42, m._43 };

            Quaternion q = Quaternion::CreateFromYawPitchRoll(
                srt.rot.y, srt.rot.x, srt.rot.z);
            Vector3 rotated = Vector3::Transform(local, q);
            Vector3 scaled = rotated * srt.scale;
            Vector3 world = scaled + srt.pos;

            return world;
        }
    }

   

    return Vector3::Zero;

}

//Vector3 CStaticMeshRenderer::LogBoneWorldPosition(int cr, const SRT& srt)
//{
//    if (!m_pScene)
//        return Vector3::Zero;  // シーン未設定時は(0,0,0)返却
//
//    auto mesh = m_pScene->mMeshes[0];           // メッシュ 0番目
//    int boneCount = mesh->mNumBones;           //ボーンの合計数(ルートボーン込み)
//
//    if (cr < 0 || cr >= boneCount) return Vector3::Zero;//外側参照もしくはルートボーン参照しているなら0
//
//    // aiBone→aiNode
//    const aiBone* bone = mesh->mBones[cr];
//    aiNode* node = m_pScene->mRootNode->FindNode(bone->mName);
//    if (!node) return Vector3::Zero;//目的のノードがない場合も0
//
//    // Assimp 上の global 行列
//    aiMatrix4x4 globalAiM = GetGlobalAiMatrix(node);
//
//    // オブジェクト SRT をかけて最終ワールド座標を計算
//    Vector3 rawPos{
//        globalAiM.a4,
//        globalAiM.b4,
//        globalAiM.c4
//    };
//    // 回転：YawPitchRoll → Quaternion
//    auto q = Quaternion::CreateFromYawPitchRoll(
//        srt.rot.y, srt.rot.x, srt.rot.z
//    );
//    Vector3 rotated = Vector3::Transform(rawPos, q);
//    Vector3 scaled = rotated * srt.scale;
//    Vector3 world = scaled + srt.pos;
//
//    return world;
//}

//void CStaticMeshRenderer::ComputeModelAABB(const aiScene* scene, aiVector3D& outMin, aiVector3D& outMax)
//{
//    // 初期化
//    const float INF = std::numeric_limits<float>::infinity();
//    outMin = aiVector3D(INF, INF, INF);
//    outMax = aiVector3D(-INF, -INF, -INF);
//
//    // 全メッシュを走査
//    for (unsigned m = 0; m < scene->mNumMeshes; ++m) {
//        const aiMesh* mesh = scene->mMeshes[m];
//        //全インデックスでループ
//        for (unsigned v = 0; v < mesh->mNumVertices; ++v) {
//            const aiVector3D& p = mesh->mVertices[v];
//            outMin.x = std::min(outMin.x, p.x);
//            outMin.y = std::min(outMin.y, p.y);
//            outMin.z = std::min(outMin.z, p.z);
//            outMax.x = std::max(outMax.x, p.x);
//            outMax.y = std::max(outMax.y, p.y);
//            outMax.z = std::max(outMax.z, p.z);
//        }
//    }
//
//}

void CStaticMeshRenderer::ApplyAnimationToNode(const aiNode* node, const Matrix4x4& parentWorld, const SRT srt, const std::unordered_map<std::string, Matrix4x4>& latestNodeTransforms, const Color& boneColor)
{
    // ノード名で検索
    auto it = latestNodeTransforms.find(node->mName.C_Str());
    Matrix4x4 local = Matrix4x4::Identity;
    if (it != latestNodeTransforms.end()) {
        local = it->second; // アニメ適用済みのローカル行列
    }
    else {
        local = ToSM(node->mTransformation); // フォールバック(bind pose)
    }

    //モデルのSRTからマトリクス生成
    Matrix4x4 modelWorld = srt.GetMatrix();

    Matrix4x4 world = local * modelWorld;

    // 3) world行列をSRTに分解
    SRT boneSrt = srt;

    boneSrt = MatrixToSRT(world);

    //boneSrt.pos = { world._41, world._42, world._43 };
    //boneSrt.pos = { local._41 * srt.scale.x, local._42 * srt.scale.y, local._43 * srt.scale.z };

    // SetWorldMatrixしてスフィア描画
    Renderer::SetWorldMatrix(&world);
    m_BoneViz->Draw(boneSrt, boneColor);
    //m_BoneViz->Draw(srt, boneColor);

    for (UINT i = 0; i < node->mNumChildren; ++i) {
        ApplyAnimationToNode(node->mChildren[i], world, srt, latestNodeTransforms, boneColor);
        //ApplyAnimationToNode(node->mChildren[i], local, srt, latestNodeTransforms, boneColor);
    }

}

void CStaticMeshRenderer::ComputeModelAABB(aiVector3D& outMin, aiVector3D& outMax)
{
    // 初期化
    const float INF = std::numeric_limits<float>::infinity();
    outMin = aiVector3D(INF, INF, INF);
    outMax = aiVector3D(-INF, -INF, -INF);


    // メッシュがなければ何もしない
    if (!m_pMesh) return;
    //スタティックメッシュから頂点情報持ってきて値を返す
    const auto& verts = m_pMesh->GetVertices();
    if (verts.empty()) return;

    for (const auto& v : verts) {
        outMin.x = std::min(outMin.x, v.Position.x);
        outMin.y = std::min(outMin.y, v.Position.y);
        outMin.z = std::min(outMin.z, v.Position.z);

        outMax.x = std::max(outMax.x, v.Position.x);
        outMax.y = std::max(outMax.y, v.Position.y);
        outMax.z = std::max(outMax.z, v.Position.z);
    }

    //// 全メッシュを走査
    //for (unsigned m = 0; m < m_pScene->mNumMeshes; ++m) {
    //    const aiMesh* mesh = m_pScene->mMeshes[m];
    //    //全インデックスでループ
    //    for (unsigned v = 0; v < mesh->mNumVertices; ++v) {
    //        const aiVector3D& p = mesh->mVertices[v];
    //        outMin.x = std::min(outMin.x, p.x);
    //        outMin.y = std::min(outMin.y, p.y);
    //        outMin.z = std::min(outMin.z, p.z);
    //        outMax.x = std::max(outMax.x, p.x);
    //        outMax.y = std::max(outMax.y, p.y);
    //        outMax.z = std::max(outMax.z, p.z);
    //    }
    //}
}

bool CStaticMeshRenderer::FindAndLogBoneRecursive(const aiNode* node,
    const aiMatrix4x4& parentAiM,
    const std::string& targetName,
    const SRT& srt,
    Vector3& outPos)
{
    // グローバル行列を計算
    aiMatrix4x4 globalAiM = parentAiM * node->mTransformation;

    // ローカル→ワールド位置
    Vector3 local{ globalAiM.a4, globalAiM.b4, globalAiM.c4 };
    Quaternion q = Quaternion::CreateFromYawPitchRoll(
        srt.rot.y, srt.rot.x, srt.rot.z);
    Vector3 rotated = Vector3::Transform(local, q);
    Vector3 scaled = rotated * srt.scale;
    Vector3 world = scaled + srt.pos;

    // ターゲット名と一致したら outPos にセットして true
    if (node->mName.C_Str() == targetName)
    {
        outPos = world;
        return true;
    }

    // 子ノードを再帰探索
    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        if (FindAndLogBoneRecursive(
            node->mChildren[i],
            globalAiM,
            targetName,
            srt,
            outPos))
        {
            return true;
        }
    }

    return false;

}

void CStaticMeshRenderer::SetDiffuseTexture(int subsetIndex, const std::string& texPath)
{
    // 必要なら拡張
    if (subsetIndex >= m_DiffuseTextures.size()) {
        m_DiffuseTextures.resize(subsetIndex + 1);
    }

    m_DiffuseTextures[subsetIndex] = std::make_unique<CTexture>();
    m_DiffuseTextures[subsetIndex]->Load(texPath);
}