#include <vector>
#include <random>

#include "system/commontypes.h"
#include "system/renderer.h"
#include "system/CPlane.h"	
#include "system/IScene.h"
#include "system/Collision.h"
#include "system/PlaneDrawer.h"

#include "system/DebugUI.h"

#include "walls.h"

// 壁データ群(外部参照したいからヘッダーに)
static std::vector<WallData> g_Walls{};

// 壁のＳＲＴ
static void debugWallSRT()
{
	static float width = 200;		// 幅
	static float height = 10;		// 高さ

	static float angley = 0.0f;		// Y軸回転角度
	static float posx = 0.0f;		// 位置X
	static float posy = 0.0f;		// 位置Y
	static float posz = 0.0f;		// 位置Z

	static int cur = 0; //選択し
	static bool wallflag = false; //選択し

	ImGui::Begin("debug Wall SRT");

	ImGui::SliderFloat("WIDTH", &width, 1,500);
	ImGui::SliderFloat("HEIGHT", &height, 1,500);

	ImGui::SliderFloat("Y Rotation", &angley, -PI, PI);
	ImGui::SliderFloat("X POS", &posx, -100, 100);
	ImGui::SliderFloat("Y POS", &posy, -100, 100);
	ImGui::SliderFloat("Z POS", &posz, -100, 100);

	ImGui::Checkbox("Onoff", &wallflag);

	ImGui::SliderInt("WALL", &cur, 0, MAXWALLNUM - 1);

	// 幅と高さをセット（１つめの壁だけ変更可能）
	if (wallflag) 
	{
		g_Walls[cur].Width = width;
		g_Walls[cur].Height = height;

		// 向きをセット
		g_Walls[cur].Rot.y = angley;

		// 座標をセット
		g_Walls[cur].Pos.x = posx;
		g_Walls[cur].Pos.y = posy;
		g_Walls[cur].Pos.z = posz;

		// 平面の方程式を再計算（サイズ・姿勢が変わったので）
		calcplaneequation();

		// 平面情報を取得
		PLANEINFO pi = g_Walls[cur].Plane.GetPlaneInfo();

		ImGui::Text("PLANE a b c %f %f %f %f", pi.plane.a, pi.plane.b, pi.plane.c, pi.plane.d);
		ImGui::Text("PLANE v0 %f %f %f", pi.p0.x, pi.p0.y, pi.p0.z);
		ImGui::Text("PLANE v1 %f %f %f", pi.p1.x, pi.p1.y, pi.p1.z);
		ImGui::Text("PLANE v2 %f %f %f", pi.p2.x, pi.p2.y, pi.p2.z);

		float w = fabs(pi.p0.x - pi.p1.x);
		float h = fabs(pi.p0.y - pi.p1.y);
		ImGui::Text("PLANE width %f ", w);
		ImGui::Text("PLANE height %f ", h);
	}
	

	ImGui::End();

}

// 平面の方程式を再計算する
void calcplaneequation() 
{

	for (auto& wall : g_Walls)
	{
		std::vector<Vector3> wallvertex{};

		Matrix4x4 wallr = Matrix4x4::CreateRotationY(wall.Rot.y);
		Matrix4x4 wallt = Matrix4x4::CreateTranslation(wall.Pos);
		Matrix4x4 mtx = wallr * wallt;

		// ４頂点を計算
		for (unsigned int y = 0; y < 2; y++) {
			Vector3	vtx{};
			for (unsigned int x = 0; x < 2; x++) {

				// 頂点座標セット
				vtx.x = -wall.Width / 2.0f + x * wall.Width;
				vtx.y = -wall.Height / 2.0f + y * wall.Height;
				vtx.z = 0.0f;

				// 壁の回転情報を反映させる
				vtx = Vector3::Transform(vtx, mtx);

				wallvertex.push_back(vtx);
			}
		}

		// 平面の方程式を作る
		wall.Plane.MakeEquatation(wallvertex[0], wallvertex[3], wallvertex[1]);
	}
}

void Getg_Walls(std::vector<WallData> vecWall)//g_Wallsを外に持ち出したくないから中身を変数に受け渡す
{
	vecWall.resize(g_Walls.size());//大きさをg_Wallに合わせる

	vecWall = g_Walls;

	return;
}

std::vector<WallData> GetWALLS()
{
	return g_Walls;
}

void initWalls() 
{
	// 平面描画初期化
	PlaneDrawerInit();

	g_Walls.resize(MAXWALLNUM);

	// 壁
	DebugUI::RedistDebugFunction(debugWallSRT);

	int i = 0;
	// 配置情報を決定
	for (auto& wall : g_Walls)
	{
		switch (i) 
		{
		case 0:
			// 位置情報はUIでセット
			wall.Pos = Vector3(0 , 0, 0);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 0, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 0.0f;
			break;

		case 1:
				// 位置情報はUIでセット
				wall.Pos = Vector3(-30, 0, 110);
				// 姿勢もUIでセット
				wall.Rot = Vector3(0, 3.1, 0);

				// サイズもUIでセット
				wall.Height = 10.0f;
				wall.Width = 100.0f;
				break;

		case 2:
			// 位置情報はUIでセット
			wall.Pos = Vector3(30, 0, 45);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 80, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 125.0f;
			break;

		case 3:
			// 位置情報はUIでセット
			wall.Pos = Vector3(4, 0, -20);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 0, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 60.0f;
			break;

		case 4:
			// 位置情報はUIでセット
			wall.Pos = Vector3(10, 0, -70);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 0, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 60.0f;
			break;

		case 5:
			// 位置情報はUIでセット
			wall.Pos = Vector3(-30, 0, 10);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 83.1, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 60.0f;
			break;

		case 6:
			// 位置情報はUIでセット
			wall.Pos = Vector3(-50, 0, 40);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 3.1, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 60.0f;
			break;

		case 7:
			// 位置情報はUIでセット
			wall.Pos = Vector3(-300, 0, 10);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 80, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 60.0f;
			break;

		case 8:
			// 位置情報はUIでセット
			wall.Pos = Vector3(-300, 0, 10);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 80, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 60.0f;
			break;

		case 9:
			// 位置情報はUIでセット
			wall.Pos = Vector3(-300, 0, 10);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 80, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 60.0f;
			break;

		default:
			// 位置情報はUIでセット
			wall.Pos = Vector3(0, 0, 0);
			// 姿勢もUIでセット
			wall.Rot = Vector3(0, 0, 0);

			// サイズもUIでセット
			wall.Height = 10.0f;
			wall.Width = 30.0f;
			break;
		}
		

		i++;
	}

	// 平面の方程式を作成する
	calcplaneequation();

	

}

void UpdateWalls()
{
	//for (auto& wall : g_Walls)
	//{
	//	wall.hitflag = false;//敵の衝突判定取れているか確かめたいから一次的にこうする
	//}
}

// 壁群と当たり判定を行う（壁と球のあたり判定を行う）
std::vector<WallCollision> checkWallCollision(
	float radius,					// 球の半径
	Vector3 pos,					// 現在位置 	
	Vector3 velocity)				// 速度ベクトル
{
	// 衝突している壁
	std::vector<WallCollision> hitwalls{};

	// 次の場所を求める
	Vector3 nextpos = pos + velocity;

	// 平面と球の距離を求める
	for (auto& wall : g_Walls)
	{
		PLANEINFO pi = wall.Plane.GetPlaneInfo();				// 壁の平面方程式を取得
		// 壁と中心座標の距離を求める（法線ベクトルを正規化しているので可能）
		float lng = pi.plane.a * nextpos.x + pi.plane.b * nextpos.y + pi.plane.c * nextpos.z + pi.plane.d;
		wall.hitflag = false;

		if (fabs(lng) < radius)
		// 半径以内なら衝突している可能性があるので　精密に判定する
		{
			// OOBと球の当たり判定を行う(奥行を持たせて考えるという事（今は Z=2.0 固定）)
			GM31::GE::Collision::BoundingBoxOBB obb;
			obb = GM31::GE::Collision::SetOBB(wall.Rot, wall.Pos, wall.Width, wall.Height, 2.0f);

			// 球の定義
			GM31::GE::Collision::BoundingSphere sphere(nextpos, radius);

			// 球とOBBの当たり判定
			bool sts = GM31::GE::Collision::CollisionSphereOBB(
				sphere,
				obb);

			// 衝突したので壁衝突したデータを作成
			if (sts) {
			
				WallCollision wallcollision;					// 衝突した壁の詳細情報

				wall.hitflag = true;
				wallcollision.Walldata = wall;					// 壁データ
				wallcollision.Penetration = Vector3(0, 0, 0);	// 侵入ベクトル
				wallcollision.Sliding = Vector3(0, 0, 0);		// 壁擦りベクトル

				ClosestPtPointOBB(sphere.center, obb, wallcollision.IntersectionPoint);		// 最近接点を求める
				hitwalls.push_back(wallcollision);				// ヒットした壁を追加
			}
		}
	}

	return hitwalls;
}

void DrawWalls()
{
	Renderer::DisableCulling(false);
//	Renderer::SetFillMode(D3D11_FILL_WIREFRAME);
	for (auto& wall : g_Walls) {
		if (wall.hitflag)
			PlaneDrawerDraw(wall.Rot, wall.Width, wall.Height, Color(1, 0, 0, 0.4f), wall.Pos.x, wall.Pos.y, wall.Pos.z);
		else {
			PlaneDrawerDraw(wall.Rot, wall.Width, wall.Height, Color(1, 1, 1, 0.4f), wall.Pos.x, wall.Pos.y, wall.Pos.z);
		}
	}
//	Renderer::SetFillMode(D3D11_FILL_SOLID);
	Renderer::DisableCulling(true);
}