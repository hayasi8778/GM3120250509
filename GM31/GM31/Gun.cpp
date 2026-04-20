#include "Gun.h"

M_Gun::M_Gun()
{
}

M_Gun::~M_Gun()
{
}

void M_Gun::Init()
{
	//属性
	Attribute = JOINABLE;
	MyType = GUN;
	//両腕に取り付け可能なオブジェクト
	Connectable[2] = true;
	Connectable[3] = true;

	m_mesh.Load(
		"assets/model/Gun/Gun_Testmodel2.fbx",				// モデル名
		"assets/model/Gun");						// テクスチャのパス


	//レンダラ初期化
	m_meshrenderer.Init(m_mesh);

	// シェーダーの初期化
	//m_shader.Create(
	//	"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
	//	"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー
	//		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
	//		"shader/unlitTexturePS.hlsl");			// ピクセルシェーダー

	m_Position.x += 20;
	m_Position.z += 10;

	aiVector3D minpos;
	aiVector3D maxpos;

	ModelAABB(minpos, maxpos);

	Width = maxpos.x - minpos.x;
	Height = maxpos.y - minpos.y;
	Depth = maxpos.z - minpos.z;

	m_shapecube_col = std::make_unique<Box>(Width, Height, Depth);

	//弾丸を生成する形にするので無効化しておく
	/*for (int i = 0; i < BulletMaxnum; i++)
	{
		m_bullets[i].Init();
	}*/
};

void M_Gun::Update(uint64_t deltatime)
{


	if (!adhesioing) //重力
	{
		m_Position.y -= 0.4;
	}
	if (m_Position.y < 0) //地面にめり込まないようにする
	{
		m_Position.y = 0;
	}

	//弾丸を初期生成する場合
	//for (int i = 0; i < BulletMaxnum; i++)
	//{
	//	m_bullets[i].Update(deltatime);
	//}

	if (Bullets.size() != 0) {
		
		for (int i = 0; i < Bullets.size(); i++) {
			Bullets[i]->Update(deltatime);
		}
	}


	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;


	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	// 方向ベクトル 抽出
	Right_vec = { transmtx._11, transmtx._12, transmtx._13 };
	Right_vec.Normalize();
	Up_vec = { transmtx._21, transmtx._22, transmtx._23 };
	Up_vec.Normalize();
	Forward_vec = { transmtx._31, transmtx._32, transmtx._33 };
	Forward_vec.Normalize();
};

void M_Gun::LateUpdate(uint64_t deltatime) 
{

	//使い終わった弾の消去をする
	if (Bullets.size() == 0) {
		return;
	}

	if (Bullets.size() != 0) {

		for (int i = 0; i < Bullets.size(); i++) {
			Bullets[i]->LateUpdate(deltatime);
		}
	}

	//for (int i = 0; i < Bullets.size(); i++) {
	//	if (Bullets[i]->GetAriveTime() < 0) {
	//		Bullets.erase(Bullets[i]);
	//	}
	//}

	for (auto bullet = Bullets.begin(); bullet != Bullets.end(); ) {
		// Alive_Time が尽きたら削除
		float time = (*bullet)->GetAriveTime();
		time = 0;
		if ((*bullet)->GetAriveTime() < 0) {
			bullet = Bullets.erase(bullet);  // unique_ptr が自動で delete
		}
		else {
			++bullet;
		}
	}
}

void M_Gun::Draw()
{
	//姿勢の補完をここでする
	/*m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;*/

	// SRT情報作成
	SRT srt;
	srt.pos = m_Position;			// 位置
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.scale = m_Scale;			// 拡縮

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	//m_shader.SetGPU();

	m_meshrenderer.Draw();

	//弾丸を初期生成する場合
	/*for (int i = 0; i < BulletMaxnum; i++)
	{
		m_bullets[i].Draw();
	}*/

	for (int i = 0; i < Bullets.size(); i++) {
		Bullets[i]->Draw();
	}

	Vector3 poscop = m_Position;//positionのコピーをとる
	// 弾の回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	Matrix4x4 m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	//m_Position.y += 1.0f;
	m_Position += Up_vec * 1.0f;
	m_Position -= Forward_vec * 5.0f;//元々2.0f
	Matrix4x4 transmtx = m_RotationMtx * Matrix4x4::CreateTranslation(m_Position);

	m_Position = poscop;//positionは元に戻しておく
	m_shapecube_col->Draw(transmtx, { 1.0,1.0,1.0,0.5 });

	//姿勢の補完をここでする
	/*m_Rotation.x -= 1.55;
	m_Rotation.y -= 1.55;*/
};

void M_Gun::Dispose()
{

};

void M_Gun::Adhesioing()
{

};

void M_Gun::Action(Vector3 vec)
{
	//if (bulletnum == BulletMaxnum) bulletnum = 0;

	// SRT情報作成
	SRT srt;
	srt.scale = m_Scale;			// 拡縮
	srt.rot = m_Rotation;			// 姿勢	srt.pos = m_Position;
	srt.pos = m_Position;			// 位置

	//新しい弾を作る
	std::unique_ptr<Bullet> pb = std::make_unique<Bullet>();

	Matrix4x4 world = srt.GetMatrix();
	//Vector3 forward = world.Forward();
	//多少下を狙うようにする
	vec.y -= 2;
	Vector3 forward = (vec - GetPosition());
	forward.Normalize();
	forward *= 3.0f;

	//生成したオブジェクト
	pb->SetForward(forward);
	
	//弾丸を初期生成する場合
	//m_bullets[bulletnum].SetForward(forward);


	//前向き行列取ってから姿勢補完する
	Vector3 bulletpos = m_meshrenderer.LogBoneWorldPosition("Shot", srt);

	//生成したオブジェクト
	pb->SetScale(Vector3(0.5, 0.5, 0.5));
	pb->SetRotation(m_Rotation);
	pb->SetPosition(bulletpos);


	//弾丸を初期生成する場合
	/*m_bullets[bulletnum].SetScale(Vector3(0.5, 0.5, 0.5));
	m_bullets[bulletnum].SetRotation(m_Rotation);
	m_bullets[bulletnum].SetPosition(bulletpos);*/

	//bulletnum++;

	//弾丸を追加する
	Bullets.push_back(std::move(pb));

};

void M_Gun::Reset()
{

};

int M_Gun::GetShaderNum()
{
	return 0;
}

GM31::GE::Collision::BoundingBoxOBB M_Gun::GetOBB()
{
	Vector3 poscop = m_Position;
	Vector3 rotcop = m_Rotation;
	GM31::GE::Collision::BoundingBoxOBB obb;
	/*m_Rotation.x += 1.55;
	m_Rotation.y += 1.55;*/

	m_Position += Up_vec * 1.0f;
	m_Position -= Forward_vec * 2.0f;
	obb = GM31::GE::Collision::SetOBB(
		m_Rotation,				// 姿勢（回転角度）
		m_Position,				// 中心座標（ワールド）
		Width,					// 幅
		Height,					// 高さ
		Depth);					// 奥行
	m_Position = poscop;
	m_Rotation = rotcop;

	return obb;
}

GM31::GE::Collision::BoundingBoxOBB M_Gun::GetOBB_Bullet(int num) 
{
	//弾丸を初期生成する場合
	/*if (num < BulletMaxnum) 
	{
		return  m_bullets[num].GetOBB();
	}*/

	if (num < Bullets.size())
	{
		return  Bullets[num]->GetOBB();
	}

	GM31::GE::Collision::BoundingBoxOBB colbox;

	return colbox;
}

void M_Gun::SetCollision_Bullet(int num, bool col)
{
	//弾丸を初期生成する場合
	//m_bullets[num].SetCol(col);

	if (num > Bullets.size())return;
	Bullets[num]->SetCol(col);
}

Vector3 M_Gun::GetBulletpos(int num)
{
	//弾丸を初期生成する場合
	//return m_bullets[num].GetPosition();

	//配列の外参照するなら0で返す
	if (num > Bullets.size()) return Vector3(0.0f, 0.0f, 0.0f);
	return Bullets[num]->GetPosition();
}
