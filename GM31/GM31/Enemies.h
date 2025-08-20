#pragma once
#include "system/commontypes.h"
#include "system/IScene.h"
#include "Enemy_Default.h"

constexpr int ENEMYMAX = 100;

void InitEnemies(IScene*);
void UpdateEnemies();
void DrawEnemies();
void DisposeEnemies();

// ƒƒbƒVƒ…æ“¾
CStaticMesh* GetEnemyMesh();

// ‘SRTSî•ñæ“¾
std::vector<SRT> GetAllRTS();

//std::unique_ptr <Enemy> 

//std::unique_ptr <Enemy> GetEnemy(int);

Enemy* GetEnemy(int);

void EnemyRemove(int , Vector3 , float);