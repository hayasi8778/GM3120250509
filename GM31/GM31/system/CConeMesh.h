#pragma once
#include	"CommonTypes.h"
#include	<random>
#include	"CMesh.h"

class CConeMesh : public CMesh {
	struct Face {
		unsigned int idx[3];
	};
public:
	void Init(
		int		divx,							// 分割数			
		float	radius,							// 半径	
		float	height,							// 高さ	
		Color color,		// 頂点カラー			
		bool bottomup = true);					// 下から上に向かって頂点を生成する
												//  (底面＝＞vertex true
												//  (vertex＝＞底面 false

	void CreateVertex();						// 頂点データ生成
	void CreateVertexTopDown();					// 頂点データ生成（下から上に向かって）

//	void CreateIndex();							// インデックスを生成

	// 面データ		
	std::vector<Face> m_faces;
private:
	unsigned int m_division_x = 1;
	unsigned int m_division_y = 1;

	float  m_radius = 100.0f;					// 半径	

	float  m_height = 100.0f;
	float  m_width;								// 半径から自動計算する

	Color m_color;			// 頂点カラー
};