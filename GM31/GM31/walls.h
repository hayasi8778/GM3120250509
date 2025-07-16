#pragma once

#include "system/commontypes.h"
#include "system/CPlane.h"

// 壁最大数
constexpr uint32_t MAXWALLNUM = 10;

// 壁データ
struct WallData {
	Vector3 Pos;				// 位置	
	Vector3 Rot;				// 姿勢
	float Height;				// 高さ
	float Width;				// 幅	
	CPlane Plane;				// 平面方程式
	bool hitflag = false;
};

// 衝突した壁データ
struct WallCollision {
	WallData Walldata;			// 壁データ	
	Vector3 Penetration;		// 侵入ベクトル
	Vector3 Sliding;			// 壁摺りベクトル
	Vector3 IntersectionPoint;	// 交点（最近接点）
};



// プロトタイプ宣言

void initWalls();				// 壁の初期処理
void DrawWalls();				// 壁の描画処理
void UpdateWalls();				// 壁の更新処理

// 壁との衝突をチェックする
std::vector<WallCollision> checkWallCollision(
	float radius, 
	Vector3 pos, 
	Vector3 velocity);

// 平面の方程式を再計算
void calcplaneequation();

void Getg_Walls(std::vector<WallData> vecWall);

std::vector<WallData> GetWALLS();

