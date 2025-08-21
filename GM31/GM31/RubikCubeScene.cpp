#include    <memory>
#include	"RubikCubeScene.h"

#include <algorithm>
#include <iostream>

// 角度から姿勢行列をつくる
void RubikCubeScene::debugRubikCubeRotation()
{

	ImGui::Begin("DebugRubikCube Rotation");

	ImGui::SliderFloat("X Rotation", &m_Rotation.x, 0.0f, PI);
	ImGui::SliderFloat("Y Rotation", &m_Rotation.y, 0.0f, PI);
	ImGui::SliderFloat("Z Rotation", &m_Rotation.z, 0.0f, PI);

	// 回転角度から回転行列を作成
	Matrix4x4 rotmtxX = Matrix4x4::CreateRotationX(m_Rotation.x);
	Matrix4x4 rotmtxY = Matrix4x4::CreateRotationY(m_Rotation.y);
	Matrix4x4 rotmtxZ = Matrix4x4::CreateRotationZ(m_Rotation.z);

	// 合成
	m_RotationMtx = rotmtxX * rotmtxY * rotmtxZ;

	// カメラの位置を極座標からデカルト座標に変換
	ImGui::End();
}

void RubikCubeScene::debugRubikCubeLocalRotation()
{
	Vector3 inputangle = { 0.0f,0.0f,0.0f };
	ImGui::Begin("DebugRubikCube Local Rotation");

	//前回から変化させたい回転角度を入力
	ImGui::SliderFloat("X Rotation", &inputangle.x, 0.0f, 0.01);
	ImGui::SliderFloat("Y Rotation", &inputangle.y, 0.0f, 0.01);
	ImGui::SliderFloat("Z Rotation", &inputangle.z, 0.0f, 0.01);



	//ローカル軸を取得
	Vector3 up = m_RotationMtx.Up();
	Vector3 right = m_RotationMtx.Right();
	Vector3 forward = m_RotationMtx.Forward();

	// Y軸のローカル回転角度も考慮
	Quaternion qy = Quaternion::CreateFromAxisAngle(up, inputangle.y);
	Quaternion qx = Quaternion::CreateFromAxisAngle(right, inputangle.x);
	Quaternion qz = Quaternion::CreateFromAxisAngle(forward, inputangle.z);

	m_RotationQ = m_RotationQ * qx * qy * qz;
	m_RotationMtx = Matrix4x4::CreateFromQuaternion(m_RotationQ);

	// X軸の回転フラグ管理
	for (int n = 0; n < CUBELAYERNUM; n++) {
		for (int i = 0; i < CUBELAYERNUM * CUBELAYERNUM; i++) {
			if (cubeRotationFlagBlocX[n][i]) {
				m_RotationQCop[n][i] = m_RotationQCop[n][i] * qx * qy * qz;
			}
		}
	}

	// Y軸の回転フラグ管理 (新規追加)
	for (int n = 0; n < CUBELAYERNUM; n++) {
		for (int i = 0; i < CUBELAYERNUM * CUBELAYERNUM; i++) {
			if (cubeRotationFlagBlocY[n][i]) {  // Y軸フラグ用変数
				m_RotationQCop[n][i] = m_RotationQCop[n][i] * qy;
			}
		}
	}

	// 変換行列の更新
	/*
	for (int n = 0; n < CUBELAYERNUM; n++) {
		for (int i = 0; i < CUBELAYERNUM * CUBELAYERNUM; i++) {
			if (cubeRotationFlagBlocX[n][i]) {
				m_RotationMtxCop[n][i] = Matrix4x4::CreateFromQuaternion(m_RotationQCop[n][i]);
			}
		}
	}
	*/
	for (int n = 0; n < CUBELAYERNUM; n++) {
		for (int i = 0; i < CUBELAYERNUM * CUBELAYERNUM; i++) {
			if (cubeRotationFlagBlocX[n][i] || cubeRotationFlagBlocY[n][i]) {
				m_RotationMtxCop[n][i] = Matrix4x4::CreateFromQuaternion(m_RotationQCop[n][i]);
			}
		}
	}




	ImGui::End();

	ImGui::Begin("DebugRubikCube Checkbox");
	//回転させるかのフラグをデバック用で作る

	const char* SerectX[] = { "NULL", "X 0", "X 1", "X 2", "X ALL", "Y 0", "Y 1", "Y 2", "Y ALL" };
	static int selectedX = 0;

	ImGui::Combo("Select Item", &selectedX, SerectX, IM_ARRAYSIZE(SerectX));

	switch (selectedX)
	{
	default:
		break;

	case 0:
		std::fill(std::begin(cubeRotationFlagBlocX[0]), std::end(cubeRotationFlagBlocX[CUBELAYERNUM - 1]), false);
		std::fill(std::begin(cubeRotationFlagBlocY[0]), std::end(cubeRotationFlagBlocY[CUBELAYERNUM - 1]), false);
		break;

	case 1:
		std::fill(std::begin(cubeRotationFlagBlocX[0]), std::end(cubeRotationFlagBlocX[0]), true);
		break;
	case 2:
		std::fill(std::begin(cubeRotationFlagBlocX[1]), std::end(cubeRotationFlagBlocX[1]), true);
		break;
	case 3:
		std::fill(std::begin(cubeRotationFlagBlocX[2]), std::end(cubeRotationFlagBlocX[2]), true);
		break;
	case 4:
		std::fill(std::begin(cubeRotationFlagBlocX[0]), std::end(cubeRotationFlagBlocX[CUBELAYERNUM - 1]), true);
		break;

	case 5:
		std::fill(std::begin(cubeRotationFlagBlocY[0]), std::end(cubeRotationFlagBlocY[0]), true);
		break;
	case 6:
		std::fill(std::begin(cubeRotationFlagBlocY[1]), std::end(cubeRotationFlagBlocY[1]), true);
		break;
	case 7:
		std::fill(std::begin(cubeRotationFlagBlocY[2]), std::end(cubeRotationFlagBlocY[2]), true);
		break;
	case 8:
		std::fill(std::begin(cubeRotationFlagBlocY[0]), std::end(cubeRotationFlagBlocY[CUBELAYERNUM - 1]), true);
		break;

	}


	ImGui::End();

}

void RubikCubeScene::debugRubikCubeRotationTest() 
{
	Vector3 inputangle = { 0.0f, 0.0f, 0.0f };
	ImGui::Begin("DebugRubikCube Local Rotation");

	ImGui::SliderFloat("X Rotation", &inputangle.x, 0.0f, 0.01);
	ImGui::SliderFloat("Y Rotation", &inputangle.y, 0.0f, 0.01);
	ImGui::SliderFloat("Z Rotation", &inputangle.z, 0.0f, 0.01);

	Vector3 up = m_RotationMtx.Up();
	Vector3 right = m_RotationMtx.Right();
	Vector3 forward = m_RotationMtx.Forward();

	Quaternion qx = Quaternion::CreateFromAxisAngle(right, inputangle.x);
	Quaternion qy = Quaternion::CreateFromAxisAngle(up, inputangle.y);

	for (int x = 0; x < CUBELAYERNUM; x++) {
		for (int y = 0; y < CUBELAYERNUM; y++) {
			for (int z = 0; z < CUBELAYERNUM; z++) {

				// X軸の回転（Y-Z平面）
				if (cubeRotationFlagBloc_X[x][y][z]) {
					m_RotationQCop_XYZ[x][y][z] = m_RotationQCop_XYZ[x][y][z] * qx;
				}

				// Y軸の回転（X-Z平面）
				if (cubeRotationFlagBloc_Y[x][y][z]) {
					m_RotationQCop_XYZ[x][y][z] = m_RotationQCop_XYZ[x][y][z] * qy;
				}

				if (cubeRotationFlagBloc_X[x][y][z] || cubeRotationFlagBloc_Y[x][y][z]) {
					m_RotationMtxCop_XYZ[x][y][z] = Matrix4x4::CreateFromQuaternion(m_RotationQCop_XYZ[x][y][z]);
				}
			}
		}
	}

	ImGui::End();

	ImGui::Begin("DebugRubikCube Checkbox");
	//回転させるかのフラグをデバック用で作る

	const char* SelectItems[] = { "NULL", "X 0", "X 1", "X 2", "X ALL", "Y 0", "Y 1", "Y 2", "Y ALL" };
	static int selectedX = 0;

	ImGui::Combo("Select Item", &selectedX, SelectItems, IM_ARRAYSIZE(SelectItems));

	// フラグ初期化
	for (int x = 0; x < CUBELAYERNUM; x++)
		for (int y = 0; y < CUBELAYERNUM; y++)
			for (int z = 0; z < CUBELAYERNUM; z++) {
				cubeRotationFlagBloc_X[x][y][z] = false;
				cubeRotationFlagBloc_Y[x][y][z] = false;
			}

	// レイヤー選択の処理
	auto applyFlagForLayer = [&](Vector3 axisDirection, int layerIndex, bool isX) {
		std::vector<std::pair<float, std::tuple<int, int, int>>> dotList;

		for (int x = 0; x < CUBELAYERNUM; x++) {
			for (int y = 0; y < CUBELAYERNUM; y++) {
				for (int z = 0; z < CUBELAYERNUM; z++) {

					Matrix4x4 mtx = Matrix4x4::CreateFromQuaternion(m_RotationQCop_XYZ[x][y][z]);
					Vector3 localAxis = Vector3::TransformNormal(axisDirection, mtx);

					Vector3 pos = Vector3(
						-(CUBELAYERNUM - 1) * 0.5f * CUBE_SIZE + x * CUBE_SIZE,
						-(CUBELAYERNUM - 1) * 0.5f * CUBE_SIZE + y * CUBE_SIZE,
						-(CUBELAYERNUM - 1) * 0.5f * CUBE_SIZE + z * CUBE_SIZE
					);

					float dot = localAxis.Dot(pos);
					dotList.push_back({ dot, {x, y, z} });
				}
			}
		}

		// ソートしてレイヤー位置を確定
		std::sort(dotList.begin(), dotList.end(),
			[](auto& a, auto& b) { return a.first < b.first; });

		// 同じ面ごとに3レイヤーに分ける
		int cubesPerLayer = CUBELAYERNUM * CUBELAYERNUM;
		float threshold = 0.01f;

		std::vector<float> dotValues;
		for (auto& pair : dotList)
			dotValues.push_back(pair.first);

		// 3つの平均を取得
		float layerDots[3];
		for (int i = 0; i < 3; i++) {
			layerDots[i] = dotValues[i * cubesPerLayer + cubesPerLayer / 2];
		}

		for (auto& [dot, idx] : dotList) {
			int x, y, z;
			std::tie(x, y, z) = idx;
			for (int i = 0; i < 3; i++) {
				if (fabs(dot - layerDots[i]) < threshold) {
					if (isX)
						cubeRotationFlagBloc_X[x][y][z] = (i == layerIndex || layerIndex == -1); // -1 = ALL
					else
						cubeRotationFlagBloc_Y[x][y][z] = (i == layerIndex || layerIndex == -1);
					break;
				}
			}
		}
		};

	//再度回転させるためのGPT製コード
	//回転後に
	//static Vector3 inputangle = { 0.0f, 0.0f, 0.0f };
		// copyと現在の回転状態から「最も近い位置」を探してフラグを立てる
	
	auto applyDynamicRotationMatch = [&](char axis, int layerIndex) {
		for (int i = 0; i < CUBELAYERNUM; i++) {
			for (int j = 0; j < CUBELAYERNUM; j++) {
				Quaternion targetQ;
				if (axis == 'X') targetQ = m_RotationQCop_XYZ_copy[layerIndex][i][j];
				if (axis == 'Y') targetQ = m_RotationQCop_XYZ_copy[i][layerIndex][j];

				float maxDot = -1.0f;
				int bestX = -1, bestY = -1, bestZ = -1;

				for (int x = 0; x < CUBELAYERNUM; x++) {
					for (int y = 0; y < CUBELAYERNUM; y++) {
						for (int z = 0; z < CUBELAYERNUM; z++) {
							float dot = targetQ.Dot(m_RotationQCop_XYZ[x][y][z]);
							if (dot > maxDot) {
								maxDot = dot;
								bestX = x;
								bestY = y;
								bestZ = z;
							}
						}
					}
				}

				if (bestX != -1) {
					if (axis == 'X') cubeRotationFlagBloc_X[bestX][bestY][bestZ] = true;
					if (axis == 'Y') cubeRotationFlagBloc_Y[bestX][bestY][bestZ] = true;
				}
			}
		}
		};
	


	switch (selectedX) {
	case 0:  // NULL（ロック解除＆回転状態復元）
	{
		for (int layer = 0; layer < CUBELAYERNUM; layer++) {
			for (int y = 0; y < CUBELAYERNUM; y++) {
				for (int z = 0; z < CUBELAYERNUM; z++) {
					Quaternion& targetQ = m_RotationQCop_XYZ_copy[1][y][z]; // X1レイヤーと仮定（必要なら可変に）

					float maxDot = -1.0f;
					int bestX = -1, bestY = -1, bestZ = -1;

					// 最も近い m_RotationQCop を探す
					for (int x2 = 0; x2 < CUBELAYERNUM; x2++) {
						for (int y2 = 0; y2 < CUBELAYERNUM; y2++) {
							for (int z2 = 0; z2 < CUBELAYERNUM; z2++) {
								float dot = targetQ.Dot(m_RotationQCop_XYZ[x2][y2][z2]);
								if (dot > maxDot) {
									maxDot = dot;
									bestX = x2;
									bestY = y2;
									bestZ = z2;
								}
							}
						}
					}

					// 最も近かったものに初期状態の位置＆回転をスナップ
					if (bestX != -1) {
						m_RotationQCop_XYZ[bestX][bestY][bestZ] = targetQ;
						m_RotationMtxCop_XYZ[bestX][bestY][bestZ] = Matrix4x4::CreateFromQuaternion(targetQ);

						// 回転フラグ解除
						cubeRotationFlagBloc_X[bestX][bestY][bestZ] = false;
						cubeRotationFlagBloc_Y[bestX][bestY][bestZ] = false;
					}
				}
			}
		}

		inputangle = Vector3(0, 0, 0);  // 入力角度もリセット
	}
	break;
	/*
	case 1: applyFlagForLayer(Vector3(1, 0, 0), 0, true); break;
	case 2: applyFlagForLayer(Vector3(1, 0, 0), 1, true); break;
	case 3: applyFlagForLayer(Vector3(1, 0, 0), 2, true); break;
	case 4: applyFlagForLayer(Vector3(1, 0, 0), -1, true); break; // X ALL

	case 5: applyFlagForLayer(Vector3(0, 1, 0), 0, false); break;
	case 6: applyFlagForLayer(Vector3(0, 1, 0), 1, false); break;
	case 7: applyFlagForLayer(Vector3(0, 1, 0), 2, false); break;
	case 8: applyFlagForLayer(Vector3(0, 1, 0), -1, false); break; // Y ALL
	*/

	case 1: applyFlagForLayer(Vector3(1, 0, 0), 0, true); applyDynamicRotationMatch('X', 0); break;
	case 2: applyFlagForLayer(Vector3(1, 0, 0), 1, true); applyDynamicRotationMatch('X', 1); break;
	case 3: applyFlagForLayer(Vector3(1, 0, 0), 2, true); applyDynamicRotationMatch('X', 2); break;
	case 4: applyFlagForLayer(Vector3(1, 0, 0), -1, true); break;

	case 5: applyFlagForLayer(Vector3(0, 1, 0), 0, false); applyDynamicRotationMatch('Y', 0); break;
	case 6: applyFlagForLayer(Vector3(0, 1, 0), 1, false); applyDynamicRotationMatch('Y', 1); break;
	case 7: applyFlagForLayer(Vector3(0, 1, 0), 2, false); applyDynamicRotationMatch('Y', 2); break;
	case 8: applyFlagForLayer(Vector3(0, 1, 0), -1, false); break;

	}

	ImGui::End();
}


void RubikCubeScene::debugRubikCubeRotation_90Rot()
{
	static int selectedFace = 0;
	static float rotationSpeed = 0.05f;

	struct RotationState {
		bool active = false;
		float totalAngle = 0.0f;
		char axis = ' ';
		int layer = -1;
	};
	static RotationState rotState;

	// === GUI ===
	ImGui::Begin("DebugRubikCube Checkbox");
	const char* items[] = { "NULL", "X 0", "X 1", "X 2", "X ALL", "Y 0", "Y 1", "Y 2", "Y ALL" ,"BOM"};
	if (ImGui::Combo("Select", &selectedFace, items, IM_ARRAYSIZE(items))) {
		rotState = {};
		switch (selectedFace) {
		case 0:  CUBE_DISTANCE = 1; BOMFLAG = false; break;//爆発フラグ切って初期化させとく
		case 1: rotState = { true, 0.0f, 'X', 0 }; break;
		case 2: rotState = { true, 0.0f, 'X', 1 }; break;
		case 3: rotState = { true, 0.0f, 'X', 2 }; break;
		case 4: rotState = { true, 0.0f, 'X', -1 }; break;
		case 5: rotState = { true, 0.0f, 'Y', 0 }; break;
		case 6: rotState = { true, 0.0f, 'Y', 1 }; break;
		case 7: rotState = { true, 0.0f, 'Y', 2 }; break;
		case 8: rotState = { true, 0.0f, 'Y', -1 }; break;
		case 9: BOMFLAG = true; break;
		default: break;
		}
	}
	ImGui::End();

	//XM_PIDIV2がDirectXMathの関数だからとりあえず同じものを作っておく
	constexpr float XM_PIDIV2 = 1.57079632679f; // π / 2

	// === 回転アニメーション ===
	//元角度から90度回った状態になるまで実行
	if (rotState.active && rotState.totalAngle < XM_PIDIV2) {
		float delta = rotationSpeed;
		if (rotState.totalAngle + delta > XM_PIDIV2)
			delta = XM_PIDIV2 - rotState.totalAngle;

		for (int x = 0; x < CUBELAYERNUM; x++) {
			for (int y = 0; y < CUBELAYERNUM; y++) {
				for (int z = 0; z < CUBELAYERNUM; z++) {
					Vector3 ref = m_Rotation_It[x][y][z];
					int rx = (int)ref.x;
					int ry = (int)ref.y;
					int rz = (int)ref.z;

					bool match = false;
					if (rotState.axis == 'X') match = (rotState.layer == -1 || x == rotState.layer);
					if (rotState.axis == 'Y') match = (rotState.layer == -1 || y == rotState.layer);

					if (!match) continue;

					Matrix4x4 localMtx = Matrix4x4::CreateFromQuaternion(m_RotationQCop_XYZ[rx][ry][rz]);
					Vector3 axis = (rotState.axis == 'X') ? localMtx.Right() : localMtx.Up();
					Quaternion rotQ = Quaternion::CreateFromAxisAngle(axis, delta);

					m_RotationQCop_XYZ[rx][ry][rz] = m_RotationQCop_XYZ[rx][ry][rz] * rotQ;
				}
			}
		}

		// **回転角を更新**
		rotState.totalAngle += delta;

		// **回転完了時の処理を追加**

		if (rotState.totalAngle >= XM_PIDIV2) {
			rotState.active = false;

			// [1] 位置情報を更新
			UpdateRotationState(rotState.axis, rotState.layer);

			// [2] 色の移動処理
			/*
			Color cubeColorCopy[CUBELAYERNUM][CUBELAYERNUM][CUBELAYERNUM];
			for (int x = 0; x < CUBELAYERNUM; x++)
				for (int y = 0; y < CUBELAYERNUM; y++)
					for (int z = 0; z < CUBELAYERNUM; z++)
						cubeColorCopy[x][y][z] = m_Cube_Color[x][y][z];//コピーをとって

			for (int x = 0; x < CUBELAYERNUM; x++)
				for (int y = 0; y < CUBELAYERNUM; y++)
					for (int z = 0; z < CUBELAYERNUM; z++) {
						Vector3 ref = m_Rotation_It[x][y][z];
						int rx = static_cast<int>(ref.x);
						int ry = static_cast<int>(ref.y);
						int rz = static_cast<int>(ref.z);
						m_Cube_Color[x][y][z] = cubeColorCopy[rx][ry][rz];//移動させる
					}
			*/
					
			// [3] 姿勢を初期化（回転角リセット）
			for (int x = 0; x < CUBELAYERNUM; x++)
				for (int y = 0; y < CUBELAYERNUM; y++)
					for (int z = 0; z < CUBELAYERNUM; z++) {
						m_RotationQCop_XYZ[x][y][z] = Quaternion::Identity;
						m_RotationMtxCop_XYZ[x][y][z] = Matrix4x4::CreateFromQuaternion(m_RotationQCop_XYZ[x][y][z]);
					}

			// --- [4] デバッグログ ---
			std::cout << "Updated Rotation Positions:\n";
			for (int x = 0; x < CUBELAYERNUM; x++) {
				for (int y = 0; y < CUBELAYERNUM; y++) {
					for (int z = 0; z < CUBELAYERNUM; z++) {
						std::cout << "[" << x << "][" << y << "][" << z << "] -> "
							<< m_Rotation_It[x][y][z].x << ", "
							<< m_Rotation_It[x][y][z].y << ", "
							<< m_Rotation_It[x][y][z].z << std::endl;
					}
				}
			}

			//回転角も初期化しておく
			for (int x = 0; x < CUBELAYERNUM; x++)
				for (int y = 0; y < CUBELAYERNUM; y++)
					for (int z = 0; z < CUBELAYERNUM; z++) {
						m_RotationQCop_XYZ[x][y][z] = m_RotationQCop_XYZ_copy[x][y][z];
					}

		}

	}


	// === 描画 ===
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 3; z++) {
				Vector3 ref = m_Rotation_It[x][y][z];
				int rx = (int)ref.x, ry = (int)ref.y, rz = (int)ref.z;
				Matrix4x4 mtx = Matrix4x4::CreateFromQuaternion(m_RotationQCop_XYZ[rx][ry][rz]);
				m_RotationMtxCop_XYZ[x][y][z] = mtx;
				// → mtx を用いて描画処理を行う（例：DrawCube(x, y, z, mtx)）
			}
		}
	}

}

RubikCubeScene::RubikCubeScene()
{

}

void RubikCubeScene::update(uint64_t deltatime)
{
	if (BOMFLAG) 
	{
		CUBE_DISTANCE += 0.30;
	}
}

void RubikCubeScene::draw(uint64_t deltatime)
{
	m_camera.Draw();

	// 3軸カラー
	Color axiscol[3] = {
		Color(1, 0, 0, 1), 
		Color(0, 1, 0, 1),
		Color(0, 1, 1, 1)
	};

	// 3本のローカル軸を描画
	for (int cnt=0;cnt<m_segments.size();cnt++)
	{
		m_segments[cnt]->Draw(m_RotationMtx, axiscol[cnt]);
	}

	// ルービックキューブを描画		
	//m_shapecube->Draw(m_RotationMtx, Color(1, 1, 1, 0.6f));

	for (int x = 0; x < CUBELAYERNUM; x++) 
	{
		for (int y = 0; y < CUBELAYERNUM; y++) 
		{
			for (int z = 0; z < CUBELAYERNUM; z++) 
			{
				float posx;
				float posy;
				float posz;

				//CUBE_DISTANCE = 1.1;

				posx = -(CUBELAYERNUM - 1) * 0.5 * CUBE_SIZE + x * CUBE_SIZE * CUBE_DISTANCE;
				posy = -(CUBELAYERNUM - 1) * 0.5 * CUBE_SIZE + y * CUBE_SIZE * CUBE_DISTANCE;
				posz = -(CUBELAYERNUM - 1) * 0.5 * CUBE_SIZE + z * CUBE_SIZE * CUBE_DISTANCE;

				Matrix4x4 transmtx = Matrix4x4::CreateTranslation(posx, posy, posz);
				//Matrix4x4 worldmtx = transmtx * m_RotationMtxCop[x][y+z];
				Matrix4x4 worldmtx = transmtx * m_RotationMtxCop_XYZ[x][y][z];

				//if (x == 0)worldmtx = transmtx * m_RotationMtxCop[0][0];
				//if (x == 1)worldmtx = transmtx * m_RotationMtxCop[0][1];
				//if (x == 2)worldmtx = transmtx * m_RotationMtxCop[0][2];


				//ルービックキューブの各面を描画
				m_shapecube->Draw(worldmtx, m_Cube_Color[x][y][z]);
				if (y == 2) 
				{
					//m_shapecube->Draw(worldmtx, Color(1, 1, 1, 0.6f));
				}
				else 
				{
					//m_shapecube->Draw(worldmtx, m_Cube_Color[x][y][z]);
				}
				
			}
		}
	}
}


void RubikCubeScene::init()
{
	// カメラ(3D)の初期化
	m_camera.Init();

	// ボックスの初期化
	m_shapecube = std::make_unique<Box>(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);

	// ローカル軸表示用線分の初期化
	m_segments[0] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(CUBE_SIZE * 10, 0,				0));
	m_segments[1] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0,				CUBE_SIZE * 10, 0));
	m_segments[2] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0,				0,				CUBE_SIZE * 10));

	//クォータニオンに単位クォータニオンをセット
	m_RotationQ = Quaternion::Identity;

	for (int x = 0; x < CUBELAYERNUM; x++)
		for (int y = 0; y < CUBELAYERNUM; y++)
			for (int z = 0; z < CUBELAYERNUM; z++) {
				m_RotationQCop_XYZ[x][y][z] = Quaternion::Identity;
				m_RotationQCop_XYZ_copy[x][y][z] = Quaternion::Identity;
				m_RotationMtxCop_XYZ[x][y][z] = Matrix4x4::CreateFromQuaternion(m_RotationQCop_XYZ[x][y][z]);
				//m_Cube_Color[x][y][z] = Color{ 0.3f ,0.3f,0.3f *(z + 1),1.0};
				m_Cube_Color[x][y][z] = Color{ 1.0f ,1.0f,1.0f,0.5 };
			}
	m_Cube_Color[0][0][0] = Color{ 0.3f,0.3f,0.3f,0.6 };

	// === 初期化 ===
	
	for (int x = 0; x < CUBELAYERNUM; x++)
		for (int y = 0; y < CUBELAYERNUM; y++)
			for (int z = 0; z < CUBELAYERNUM; z++)
				m_Rotation_It[x][y][z] = Vector3((float)x, (float)y, (float)z);
	

	DebugUI::RedistDebugFunction([this]() {
		debugRubikCubeRotation_90Rot();
		});

}


void RubikCubeScene::dispose()
{

}

int RubikCubeScene::ChangeScene()
{
	return 0;
}

void RubikCubeScene::UpdateRotationState(char axis, int layer) {
	// --- ① ALLレイヤー（layer == -1）の場合 ---
	if (layer == -1) {
		for (int l = 0; l < 3; l++) {
			UpdateRotationState(axis, l);
		}
		return;
	}

	// --- ② 個別レイヤーの処理（既存処理） ---
	Vector3 tmp[3][3];
	Color tmpColor[3][3];

	if (axis == 'X') {
		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 3; z++) {
				tmp[y][z] = m_Rotation_It[layer][y][z];
				tmpColor[y][z] = m_Cube_Color[layer][y][z];
			}
		}

		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 3; z++) {
				int ny = 2 - z;
				int nz = y;
				m_Rotation_It[layer][ny][nz] = tmp[y][z];
				m_Rotation_It[layer][ny][nz].x = layer;
				m_Rotation_It[layer][ny][nz].y = ny;
				m_Rotation_It[layer][ny][nz].z = nz;

				m_Cube_Color[layer][ny][nz] = tmpColor[y][z];
			}
		}
	}
	else if (axis == 'Y') {
		for (int x = 0; x < 3; x++) {
			for (int z = 0; z < 3; z++) {
				tmp[x][z] = m_Rotation_It[x][layer][z];
				tmpColor[x][z] = m_Cube_Color[x][layer][z];
			}
		}

		for (int x = 0; x < 3; x++) {
			for (int z = 0; z < 3; z++) {
				int nx = z;
				int nz = 2 - x;
				m_Rotation_It[nx][layer][nz] = tmp[x][z];
				m_Rotation_It[nx][layer][nz].x = nx;
				m_Rotation_It[nx][layer][nz].y = layer;
				m_Rotation_It[nx][layer][nz].z = nz;

				m_Cube_Color[nx][layer][nz] = tmpColor[x][z];
			}
		}
	}
	else if (axis == 'Z') {
		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				tmp[x][y] = m_Rotation_It[x][y][layer];
				tmpColor[x][y] = m_Cube_Color[x][y][layer];
			}
		}

		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				int nx = 2 - y;
				int ny = x;
				m_Rotation_It[nx][ny][layer] = tmp[x][y];
				m_Rotation_It[nx][ny][layer].x = nx;
				m_Rotation_It[nx][ny][layer].y = ny;
				m_Rotation_It[nx][ny][layer].z = layer;

				m_Cube_Color[nx][ny][layer] = tmpColor[x][y];
			}
		}
	}


}
