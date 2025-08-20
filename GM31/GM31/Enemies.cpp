#include "system/commontypes.h"
#include <vector>
#include <memory>
#include <random>
#include "system/CStaticMesh.h"
#include "system/CStaticMeshRenderer.h"
#include "system/CShader.h"
#include "Enemies.h"
#include "Enemy_Default.h"

// 敵多数
std::vector<std::unique_ptr<Enemy>> g_Enemies;

// 敵のメッシュデータ
static CStaticMesh g_EnemyMesh{};
static CStaticMeshRenderer g_EnemyMeshRenderer{};

// シェーダー
static CShader g_Shader{};	

// メッシュ
CStaticMesh* GetEnemyMesh() 
{
	return &g_EnemyMesh;
}

void InitEnemies(IScene* currentscene)
{
	std::mt19937 mt{ std::random_device{}() };
	std::uniform_real_distribution<float> posdist{ -500.0f, 500.0f };

	std::uniform_real_distribution<float> rotdist{ 0.0f, PI };

	// モデルの初期化
	g_EnemyMesh.Load(
		"assets/model/car001.x",
		"assets/model/");

	// レンダラ初期化
	g_EnemyMeshRenderer.Init(g_EnemyMesh);

	// シェーダーの初期化
	g_Shader.Create(
		"shader/vertexLightingVS.hlsl",			// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");		// ピクセルシェーダー

	for(int i = 0; i < ENEMYMAX; i++)
	{
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(currentscene);
		enemy->Init();
		enemy->SetPosition(Vector3(posdist(mt), 0.0f, posdist(mt)));
		enemy->SetRotation(Vector3(0.0, rotdist(mt), 0.0));
		enemy->SetMeshRenderer(&g_EnemyMeshRenderer);
		g_Enemies.emplace_back(std::move(enemy));
	}

}

void UpdateEnemies() 
{
	for (auto& e : g_Enemies)
	{
		e->Update();
	}
}

void DrawEnemies() 
{

	g_Shader.SetGPU();

	for (auto& e:g_Enemies)
	{
		e->Draw();	
	}
}

void DisposeEnemies() 
{

}

std::vector<SRT> GetAllRTS() 
{
	std::vector<SRT> allrts;
	for (auto& e : g_Enemies)
	{
		SRT r;
		r.pos = e->GetPosition();
		r.rot = e->GetRotation();
		r.scale = e->GetScale();
		allrts.emplace_back(r);
	}

	return allrts;
}

//std::unique_ptr <Enemy> GetEnemy(int i)
//{
//	return g_Enemies;
//}

Enemy* GetEnemy(int i)
{
	return g_Enemies[i].get();
}

void EnemyRemove(int i ,Vector3 rot , float Wallsize)
{
	g_Enemies[i]->Remove(rot,Wallsize);
}
