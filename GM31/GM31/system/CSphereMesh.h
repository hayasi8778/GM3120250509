#pragma once
#include	"CommonTypes.h"
#include	"CMesh.h"


class CSphereMesh : public CMesh {
	// 法線ベクトルを計算
// 頂点座標をノーマライズ
	void Normalize(Vector3 vec, Vector3& Normal) {
		vec.Normalize();
		Normal = vec;
	}

public:
	void Init(float radius,
		Color color,
		int division_horizontal, 
		int division_vertical) 
	{
		// 分割数を保存
		m_divX = division_horizontal;
		m_divY = division_vertical;

		// 半径を保存
		m_radius = radius;

		// カラー値を保存
		m_color = color;

		// 球のインデックスデータを作成
		CreateIndex();

		// 球の頂点データを作成
		CreateVertex();
	}

	void CreateVertex() {
		// 頂点データクリア
		m_vertices.clear();

		float azimuth = 0.0f;			// 方位角
		float elevation = 0.0f;			// 仰角

		Vector3	Normal;

		// 方位角と仰角から球メッシュの頂点データを作成
		for (unsigned int y = 0; y <= m_divY; y++) {
			elevation = (PI * (float)y) / (float)m_divY;    // 仰角をセット
			float r = m_radius * sinf(elevation);						// 仰角に応じた半径を計算

			for (unsigned int x = 0; x <= m_divX; x++) {
				azimuth = (2 * PI * (float)x) / (float)m_divX;	// 方位角をセット

				VERTEX_3D v;
				v.Position.x = r * cosf(azimuth);
				v.Position.y = m_radius * cosf(elevation);
				v.Position.z = r * sinf(azimuth);

				Normalize(v.Position, Normal);		// 法線を計算
				v.Normal = Normal;					// 法線をセット

				v.Diffuse = m_color;				// 頂点カラー

				m_vertices.emplace_back(v);
			}
		}
	}

	void CreateIndex() {
		struct FACE {
			unsigned int idx[3];
		};

		// インデックスデータクリア
		m_indices.clear();

		// インデックス生成
		for (unsigned int y = 0; y < m_divY; y++) {
			for (unsigned int x = 0; x < m_divX; x++) {
				int count = (m_divX + 1) * y + x;			// 左上座標のインデックス

				FACE f;
				// 上半分
				f.idx[0] = count;
				f.idx[1] = count + 1;
				f.idx[2] = count + 1 + (m_divX + 1);

				m_indices.emplace_back(f.idx[0]);
				m_indices.emplace_back(f.idx[1]);
				m_indices.emplace_back(f.idx[2]);

				// 下半分
				f.idx[0] = count;
				f.idx[1] = count + (m_divX + 1) + 1;
				f.idx[2] = count + (m_divX + 1);

				m_indices.emplace_back(f.idx[0]);
				m_indices.emplace_back(f.idx[1]);
				m_indices.emplace_back(f.idx[2]);
			}
		}
	}
private:
	unsigned int m_divX = 1;
	unsigned int m_divY = 1;
	float m_radius = 100.0f;
	Color m_color;
};


