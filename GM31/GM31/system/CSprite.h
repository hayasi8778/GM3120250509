#pragma once
#include "CommonTypes.h"
#include "renderer.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"

class CSprite 
{
	static CShader m_Shader;

	CTexture m_Texture{};
	CMaterial m_Material{};

	int m_Width = 0;
	int m_Height = 0;

	CIndexBuffer				m_IndexBuffer;
	CVertexBuffer<VERTEX_3D>	m_VertexBuffer;

	std::vector<VERTEX_3D>		m_Vertices{};

public:
	CSprite(int width, int height, std::string texfilename,Vector2 uv[4]);
	CSprite(int width, int height, std::string texfilename,Vector2 uv[4],MATERIAL mtr);
	CSprite(int width, int height, std::string texfilename, Vector2 uv[4], MATERIAL mtr, std::string vs, std::string ps);
	virtual ~CSprite() {
	}

	void Update();
	void Draw(Vector3 scale, Vector3 rotation, Vector3 pos);
	void Draw3D(Vector3 scale, Vector3 rotation, Vector3 pos);
	void Dispose();
	void ModifyUV(const Vector2* uv);
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	//フェード入れるためのテクスチャ色変更
	void SetMaterial(MATERIAL mtl) { m_Material.Create(mtl); }
};