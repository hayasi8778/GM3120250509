#include	<iostream>
#include	"CCylinderMesh.h"

// 頂点データを作成
void CCylinderMesh::Init(
	int		divx,							// 分割数			
	float	radius,							// 半径	
	float	height,							// 高さ	
	Color color)		// 頂点カラー
{
	// サイズセット（幅と高さ）（XY平面）
	m_height = height;
	m_width = 2.0f*PI*radius;		// 直径×円周率
	m_radius = radius;

	// 分割数
	m_division_x = divx;

	// 頂点カラー
	m_color = color;

	// 頂点データ生成
	CreateVertex();
}

void CCylinderMesh::CreateVertex() {

	// 頂点データクリア
	m_vertices.clear();

	// 底面の円周上の頂点データを作成
	float azimuth = 0.0f;			// 方位角

	// 側面の頂点データを作成
	for (unsigned int i = 0; i <= m_division_x; i++) {
		azimuth = (2 * PI * (float)i) / (float)m_division_x;	// 方位角をセット
		VERTEX_3D	v{};

		v.Position.x = m_radius * cosf(azimuth);
		v.Position.y = 0.0f;
		v.Position.z = m_radius * sinf(azimuth);

		v.Normal = v.Position;
		v.Normal.Normalize();			// 法線ベクトルを頂点座標と同じにする	
		v.Diffuse = m_color;

		m_vertices.emplace_back(v);
	}

	for (unsigned int i = 0; i <= m_division_x; i++) {
		azimuth = (2 * PI * (float)i) / (float)m_division_x;	// 方位角をセット
		VERTEX_3D	v{};

		v.Position.x = m_radius * cosf(azimuth);
		v.Position.y = m_height;
		v.Position.z = m_radius * sinf(azimuth);

		v.Normal = Vector3(0.0f, 1.0f, 0.0f);		// 法線ベクトルを頂点座標と同じにする	
		v.Diffuse = m_color;

		m_vertices.emplace_back(v);
	}

	// インデックスデータ生成(底面側)
	for (unsigned int i = 0; i < m_division_x; i++) {
		m_indices.push_back(i);						// 頂点
		m_indices.push_back(i + m_division_x + 2);	// 次の底面頂点
		m_indices.push_back(i + 1);					// 現在の底面頂点
	}

	// インデックスデータ生成(上面側)
	for (unsigned int i = 0; i < m_division_x; i++) {
		m_indices.push_back(i);						// 頂点
		m_indices.push_back(i + m_division_x + 1);	// 次の底面頂点
		m_indices.push_back(i + m_division_x + 2);	// 現在の底面頂点
	}

	// 底面の中心座標
	{
		VERTEX_3D v{};
		v.Position = Vector3(0.0f, 0.0f, 0.0f);
		v.Normal = Vector3(0.0f, 1.0f, 0.0f);		// 法線ベクトルを頂点座標と同じにする	
		v.Diffuse = m_color;
		m_vertices.emplace_back(v);
	}

	// 底面の中心座標のインデックス
	unsigned int bottomcenteridx = static_cast<unsigned int>(m_vertices.size() - 1);

	// インデックスデータ生成(底面)
	for (unsigned int i = 0; i < m_division_x; i++) {
		m_indices.push_back(bottomcenteridx);		// 中心
		m_indices.push_back(i);						// 次の底面頂点
		m_indices.push_back(i + 1);					// 現在の底面頂点
	}

	// 上面の中心座標
	{
		VERTEX_3D v{};
		v.Position = Vector3(0.0f, m_height, 0.0f);
		v.Normal = Vector3(0.0f, 1.0f, 0.0f);		// 法線ベクトルを頂点座標と同じにする	
		v.Diffuse = m_color;
		m_vertices.emplace_back(v);
	}

	// 底面の中心座標のインデックス
	int topcenteridx = static_cast<int>(m_vertices.size() - 1);

	// インデックスデータ生成(底面)
	for (unsigned int i = 0; i < m_division_x; i++) {
		m_indices.push_back(topcenteridx);			// 中心
		m_indices.push_back(i + m_division_x + 2);	// 次の底面頂点
		m_indices.push_back(i + m_division_x + 1);	// 現在の底面頂点
	}
}
