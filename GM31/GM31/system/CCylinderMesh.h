#pragma once
#include	"CommonTypes.h"
#include	<random>
#include	"CMesh.h"

class CCylinderMesh : public CMesh {
public:
	void Init(
		int		divx,							// 分割数			
		float	radius,							// 半径	
		float	height,							// 高さ	
		Color color);		// 頂点カラー

	void CreateVertex();						// 頂点データ生成
private:
	unsigned int m_division_x = 1;
	unsigned int m_division_y = 1;

	float  m_radius = 100.0f;					// 半径	

	float  m_height = 100.0f;
	float  m_width;								// 半径から自動計算する

	Color m_color;			// 頂点カラー
};