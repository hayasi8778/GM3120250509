#include	"CSprite.h"

CShader CSprite::m_Shader{};

CSprite::CSprite(int width, int height, std::string texfilename,Vector2 uv[4])
{
	m_Width = width;
	m_Height = height;

	// �}�e���A������
	MATERIAL	mtrl;
	mtrl.Ambient = Color(0, 0, 0, 0);
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shiness = 0;
	mtrl.TextureEnable = TRUE;

	m_Material.Create(mtrl);

	// �e�N�X�`�����[�h
	bool sts = m_Texture.Load(texfilename);
	assert(sts == true);

	// �V�F�[�_�I�u�W�F�N�g����
	m_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");

	// ���_�f�[�^
	m_Vertices.clear();
	m_Vertices.resize(4);

	m_Vertices[0].Position = Vector3(-width / 2.0f, -height / 2.0f, 0);
	m_Vertices[1].Position = Vector3( width / 2.0f, -height / 2.0f, 0);
	m_Vertices[2].Position = Vector3(-width / 2.0f, height  / 2.0f, 0);
	m_Vertices[3].Position = Vector3( width / 2.0f,  height / 2.0f, 0);

	m_Vertices[0].Diffuse = Color(1, 1, 1, 1);
	m_Vertices[1].Diffuse = Color(1, 1, 1, 1);
	m_Vertices[2].Diffuse = Color(1, 1, 1, 1);
	m_Vertices[3].Diffuse = Color(1, 1, 1, 1);

	m_Vertices[0].TexCoord = uv[0];
	m_Vertices[1].TexCoord = uv[1];
	m_Vertices[2].TexCoord = uv[2];
	m_Vertices[3].TexCoord = uv[3];

	// ���_�o�b�t�@����
	m_VertexBuffer.Create(m_Vertices);

	// �C���f�b�N�X�o�b�t�@����
	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	// �C���f�b�N�X�o�b�t�@����
	m_IndexBuffer.Create(indices);
}

CSprite::CSprite(int width, int height, std::string texfilename, Vector2 uv[4],MATERIAL mtrl)
{
	m_Width = width;
	m_Height = height;

	m_Material.Create(mtrl);

	// �e�N�X�`�����[�h
	bool sts = m_Texture.Load(texfilename);
	assert(sts == true);

	// �V�F�[�_�I�u�W�F�N�g����
	m_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");

	// ���_�f�[�^
	// ���_�f�[�^
	m_Vertices.clear();
	m_Vertices.resize(4);

	m_Vertices[0].Position = Vector3(-width / 2.0f, -height / 2.0f, 0);
	m_Vertices[1].Position = Vector3(width / 2.0f, -height / 2.0f, 0);
	m_Vertices[2].Position = Vector3(-width / 2.0f, height / 2.0f, 0);
	m_Vertices[3].Position = Vector3(width / 2.0f, height / 2.0f, 0);

	m_Vertices[0].Diffuse = Color(1, 1, 1, 1);
	m_Vertices[1].Diffuse = Color(1, 1, 1, 1);
	m_Vertices[2].Diffuse = Color(1, 1, 1, 1);
	m_Vertices[3].Diffuse = Color(1, 1, 1, 1);

	m_Vertices[0].TexCoord = uv[0];
	m_Vertices[1].TexCoord = uv[1];
	m_Vertices[2].TexCoord = uv[2];
	m_Vertices[3].TexCoord = uv[3];

	// ���_�o�b�t�@����
	m_VertexBuffer.Create(m_Vertices);

	// �C���f�b�N�X�o�b�t�@����
	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	// �C���f�b�N�X�o�b�t�@����
	m_IndexBuffer.Create(indices);
}

void CSprite::ModifyUV(const Vector2* uv) 
{
	// ���_�f�[�^
	m_Vertices[0].TexCoord = uv[0];
	m_Vertices[1].TexCoord = uv[1];
	m_Vertices[2].TexCoord = uv[2];
	m_Vertices[3].TexCoord = uv[3];

	// ���_�o�b�t�@����
	m_VertexBuffer.Modify(m_Vertices);

}


void CSprite::Update() 
{

}

void CSprite::Draw(Vector3 scale,Vector3 rotation,Vector3 pos) 
{
	// SRT���쐬
	SRT	srt;

	srt.scale = scale;
	srt.rot = rotation;
	srt.pos = pos;

	Matrix4x4 worldmtx;

	worldmtx = srt.GetMatrix();

	Renderer::SetWorldViewProjection2D();	// 2D�p�̃r���[�s����Z�b�g

	Renderer::SetWorldMatrix(&worldmtx);	// GPU�ɃZ�b�g

	// �`��̏���
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// �g�|���W�[���Z�b�g�i���v���~�e�B�u�^�C�v�j
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_Shader.SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();

	m_Material.SetGPU();
	m_Texture.SetGPU();

	devicecontext->DrawIndexed(
		4,							// �`�悷��C���f�b�N�X���i�l�p�`�Ȃ�łS�j
		0,							// �ŏ��̃C���f�b�N�X�o�b�t�@�̈ʒu
		0);

}

void CSprite::Dispose() 
{

}
