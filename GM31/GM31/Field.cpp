#include	"Field.h"

void Field::Init()
{
	// 頂点データ
	std::vector<VERTEX_3D>	vertices;

	vertices.resize(4);

	vertices[0].Position = Vector3(-500,  0,  500);
	vertices[1].Position = Vector3( 500,  0,  500);
	vertices[2].Position = Vector3(-500,  0, -500);
	vertices[3].Position = Vector3( 500,  0, -500);

	vertices[0].Diffuse = Color(1, 1, 1, 1);
	vertices[1].Diffuse = Color(1, 1, 1, 1);
	vertices[2].Diffuse = Color(1, 1, 1, 1);
	vertices[3].Diffuse = Color(1, 1, 1, 1);

	vertices[0].TexCoord = Vector2(0, 0);
	vertices[1].TexCoord = Vector2(10, 0);
	vertices[2].TexCoord = Vector2(0, 10);
	vertices[3].TexCoord = Vector2(10, 10);

	// 頂点バッファ生成
	m_VertexBuffer.Create(vertices);

	// インデックスバッファ生成
	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	// インデックスバッファ生成
	m_IndexBuffer.Create(indices);

	//// シェーダオブジェクト生成
	m_Shader.Create("shader/unlitTextureVS.hlsl","shader/unlitTexturePS.hlsl");

	// マテリアル生成
	MATERIAL	mtrl;
	mtrl.Ambient = Color(0, 0, 0, 0);
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shiness = 0;
	mtrl.TextureEnable = TRUE;

	m_Material.Create(mtrl);

	// テクスチャロード
	bool sts = m_Texture.Load("assets\\texture\\field000.jpg");
	//bool sts = m_Texture.Load("assets\\texture\\wall000.jpg");
	assert(sts == true);

	//法線の取得
	// 1) 平面上の３点を取得
	Vector3 p0 = vertices[0].Position;
	Vector3 p1 = vertices[1].Position;
	Vector3 p2 = vertices[2].Position;

	// 2) 法線を計算（右手系）
	Vector3 v1 = p1 - p0;
	Vector3 v2 = p2 - p0;
	//まずは法線を計算して vector3 を得る
	Vector3 normal = v1.Cross(v2);

	//その vector3 を正規化（Normalize() は in-place で void）
	normal.Normalize();

	//メンバに格納
	m_Normal = normal;



	// 3) 平面上の基準点として p0 を保持
	m_PointOnPlane = p0;

}


void Field::Draw()
{
	// SRT情報作成
	SRT srt;

	srt.pos = m_Position;		// 平行移動
	srt.rot = m_Rotation;		// 回転rt.pos = m_Position;		// 平行移動
	srt.scale = m_Scale;		// スケール

	// SRTを行列に変換	
	Matrix4x4 worldmtx = srt.GetMatrix();


	Renderer::SetWorldMatrix(&worldmtx);		// GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（旧プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//m_Shader.SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();

	m_Material.SetGPU();
	m_Texture.SetGPU();

	devicecontext->DrawIndexed(
		4,							// 描画するインデックス数（四角形なんで４）
		0,							// 最初のインデックスバッファの位置
		0);
}

void Field::Dispose()
{

}