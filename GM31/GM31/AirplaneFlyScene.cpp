#include    <memory>
#include    <string>
#include    "system/commontypes.h"
#include	"system/collision.h"
#include	"AirplaneFlyScene.h"
#include	"system/renderer.h"
#include    "system/CDirectInput.h"

// 平行光源の方向セット
void AirplaneFlyScene::debugDirectionalLight()
{
	static Vector4 direction = Vector4(0.0f, 0.0f,  1.0f, 0.0f); // Z軸+方向に光を当てる	

	ImGui::Begin("debug Directional Light");

	ImGui::SliderFloat3("direction ",&direction.x, -1, 1);
	direction.Normalize();										// 正規化

	LIGHT light{};
	light.Enable = true;
	light.Direction = direction;

	light.Direction.Normalize();
	light.Ambient = Color(0.2f, 0.2f, 0.2f, 1.0f);
	light.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4 Direction = Vector4(direction.x, direction.y,direction.z, 0.0f);
	Renderer::SetLight(light);

	ImGui::End();
}

// デバッグフリーカメラ
void AirplaneFlyScene::debugFreeCamera()
{
	ImGui::Begin("debug Free camera");

	static float radius = 100.0f;
	static Vector3 pos = Vector3(0, 0, radius);
	static Vector3 lookat = Vector3(0, 0, 0);
	static float elevation = -90.0f * PI / 180.0f;
	static float azimuth = PI/2.0f;

	static Vector3 spherecenter = Vector3(0, 0, 0);	

	ImGui::SliderFloat("Radius", &radius, 1,800);
	ImGui::SliderFloat("Elevation", &elevation, -PI, PI);
	ImGui::SliderFloat("Azimuth", &azimuth, -PI, PI);

	ImGui::SliderFloat3("lookat ", &lookat.x, -100, 100);

	// カメラの位置を極座標からデカルト座標に変換
	m_camera.SetRadius(radius);
	m_camera.SetElevation(elevation);
	m_camera.SetAzimuth(azimuth);
	m_camera.SetLookat(lookat);

	// カメラの位置を極座標から求める
	m_camera.CalcCameraPosition();

	ImGui::End();
}

AirplaneFlyScene::AirplaneFlyScene()
{
}

void AirplaneFlyScene::update(uint64_t deltatime)
{
	Fly();
}

void AirplaneFlyScene::draw(uint64_t deltatime)
{
	m_camera.Draw();

	// 3軸カラー
	Color axiscol[3] = {
		Color(1, 0, 0, 1),
		Color(0, 1, 0, 1),
		Color(0, 1, 1, 1)
	};

	// 3軸のワールド軸を描画
	for (int axisno = 0; axisno < 3; axisno++)
	{
		Matrix4x4 rotmtx = Matrix4x4::Identity;
		m_segments[axisno]->Draw(rotmtx, axiscol[axisno]);
	}

	// 3軸のローカル軸を描画
	for (int axisno=0; axisno <3; axisno++)
	{
		m_segments[axisno]->Draw(m_RotationMtx, axiscol[axisno]);
	}

	// スカイドームの描画
	{
		m_shaders[0]->SetGPU();
		Matrix4x4 rotmtx = Matrix4x4::Identity;
		Renderer::SetWorldMatrix(&rotmtx);
		m_meshrenders[0]->Draw();
	}

	// 飛行機の描画
	{
		m_shaders[1]->SetGPU();
		Renderer::SetWorldMatrix(&m_RotationMtx);
		m_meshrenders[1]->Draw();
	}
}

void AirplaneFlyScene::init()
{
	// カメラ(3D)の初期化
	m_camera.Init();

	// シェーダーの初期化
	m_shaders[0] = std::make_unique<CShader>();
	m_shaders[1] = std::make_unique<CShader>();

	m_shaders[0]->Create(
		"shader/unlitTextureVS.hlsl",				// 頂点シェーダー
		"shader/unlitTexturePS.hlsl");				// ピクセルシェーダー

	m_shaders[1]->Create(
		"shader/vertexLightingVS.hlsl",				// 頂点シェーダー
		"shader/vertexLightingPS.hlsl");			// ピクセルシェーダー

	// モデルの読み込み
	m_staticmeshs[0] = std::make_unique<CStaticMesh>();
	m_staticmeshs[0]->Load("assets/model/skydome.x", "assets/model/");
	m_meshrenders[0] = std::make_unique<CStaticMeshRenderer>();
	m_meshrenders[0]->Init(*m_staticmeshs[0]);

	m_staticmeshs[1] = std::make_unique<CStaticMesh>();
	m_staticmeshs[1]->Load("assets/model/f1.x", "assets/model/");
	m_meshrenders[1] = std::make_unique<CStaticMeshRenderer>();
	m_meshrenders[1]->Init(*m_staticmeshs[1]);

	// ローカル軸表示用線分の初期化
	m_segments[0] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(100, 0, 0));
	m_segments[1] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 100, 0));
	m_segments[2] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 0, 100));


	// デバッグフリーカメラ
	DebugUI::RedistDebugFunction([this]() {
		debugFreeCamera();
		});

	// デバッグLIGHT
	DebugUI::RedistDebugFunction([this]() {
		debugDirectionalLight();
		});

}

void AirplaneFlyScene::dispose()
{

}

int AirplaneFlyScene::ChangeScene()
{
	return 0;
}

void AirplaneFlyScene::Fly() 
{
	Vector3 inputangle = { 0.0f,0.0f,0.0f };
	bool inputflag = false;
	float speed = 0.4f;	// 移動スピード

	if(CDirectInput::GetInstance().CheckKeyBuffer(DIK_UP)) 
	{
		inputangle.x -= 0.01f;	// X軸回転
		inputflag = true;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_DOWN))
	{
		inputangle.x += 0.01f;	// X軸回転
		inputflag = true;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT))
	{
		inputangle.y -= 0.01f;	// Y軸回転
		inputflag = true;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT))
	{
		inputangle.y += 0.01f;	// Y軸回転
		inputflag = true;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_F1))
	{
		inputangle.z -= 0.01f;	// Y軸回転
		inputflag = true;
	}
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_F2))
	{
		inputangle.z += 0.01f;	// Y軸回転
		inputflag = true;
	}

	// 入力があった場合
	if (inputflag) {

		// ローカル軸を取得
		Vector3 up = m_RotationMtx.Up();
		Vector3 right = m_RotationMtx.Right();
		Vector3 forward = m_RotationMtx.Forward();

		Quaternion qy = Quaternion::CreateFromAxisAngle(up, inputangle.y);
		Quaternion qx = Quaternion::CreateFromAxisAngle(right, inputangle.x);
		Quaternion qz = Quaternion::CreateFromAxisAngle(forward, inputangle.z);

		// 次の姿勢＝現在の姿勢×回転クオータニオン
		m_RotationQ = m_RotationQ * qx * qy * qz;

		m_RotationMtx = Matrix4x4::CreateFromQuaternion(m_RotationQ);
	}

	// 前方に移動	
	Vector3 forward = m_RotationMtx.Forward();
	m_position += -forward * speed;

	m_RotationMtx._41 = m_position.x;	// X座標
	m_RotationMtx._42 = m_position.y;	// Y座標
	m_RotationMtx._43 = m_position.z;	// Z座標
}
