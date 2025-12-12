#include "system/commonTypes.h"
#include "system/renderer.h"
#include "camera.h"
#include "application.h"
#include <DirectXMath.h>

#include "Random.h"

using namespace DirectX;

//カメラ座標と移動目標の距離とって移動させたいのでこれ作る
//inline float GetRange(Vector3 vecA, Vector3 vecB) {
//	Vector3 coppos_A = vecA;
//	Vector3 coppos_B = vecB;
//	if (coppos_A.x < 0) coppos_A.x *= -1;
//	if (coppos_A.y < 0) coppos_A.y *= -1;
//	if (coppos_A.z < 0) coppos_A.z *= -1;
//
//	if (coppos_B.x < 0) coppos_B.x *= -1;
//	if (coppos_B.y < 0) coppos_B.y *= -1;
//	if (coppos_B.z < 0) coppos_B.z *= -1;
//	Vector3 ranged = { coppos_A.x - coppos_B.x , coppos_A.y - coppos_B.y , coppos_A.z - coppos_B.z };
//	if (ranged.x < 0) ranged.x *= -1;
//	if (ranged.y < 0) ranged.y *= -1;
//	if (ranged.z < 0) ranged.z *= -1;
//	float rangedALL = ranged.x + ranged.y + ranged.z;
//
//	return rangedALL;
//}

void Camera::Init()
{
	m_position = Vector3(0.0f, 10.0f, -100.0f);
	m_lookat = Vector3(0.0f, 10.0f, 0.0f);
}

void Camera::Dispose()
{

}


void Camera::Update()
{
	//if (GetRange(m_position, moveposition) < 10) m_position = moveposition;

	//if (moveposition != Vector3(0.0f, 0.0f, 0.0f)) {
	//	Vector3 MoveVec = moveposition - m_position;
	//	MoveVec.y = 0;//正規化前にy軸を切る
	//	MoveVec.Normalize();

	//	float ranged = GetRange(moveposition, m_position);
	//	
	//	if (ranged <= 10.0f) {//移動地点と現在地が近いなら移動しない
	//		//m_position = moveposition;
	//		//moveposition = Vector3(0.0f, 0.0f, 0.0f);
	//		if (moveposition != campos) moveposition = campos;
	//		else m_position = moveposition;
	//	}else m_position += MoveVec * 2.0f;
	//}
	//else m_position = campos;

	moveposition.y = 30;
	Vector3 MoveVec = moveposition - m_position;
	//MoveVec.y = 0;

	float dist = MoveVec.Length();

	if (dist > 0.01f)
	{
		MoveVec.Normalize();
		m_position += MoveVec * 2.0f; // スムーズに追従
	}
	
}

void Camera::Update_time(uint64_t deltatime)
{
	//振動していないなら動かさない
	if (m_vibration_time == 0) return;
	
	float time = static_cast<float>(deltatime) / 1000;
	m_vibration_time -= time;
	if (m_vibration_time < 0) {
		//振動を無効化
		m_vibration = 0;
		m_vibration_time = 0;
	}
	RandomGen rand;
	m_lookat.x += rand.UniformFloat(-m_vibration, m_vibration);
}

void Camera::LateUpdate()
{
	RandomGen rand;

	//カメラを揺らしたい
	float RanX = rand.UniformFloat(-10.0f, 10.0f);

	m_lookat.x += RanX;
}

void Camera::Draw()
{
	// ビュー変換後列作成
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	m_viewmtx = 
		DirectX::XMMatrixLookAtLH(
			m_position, 
			m_lookat, 
			up);				// 左手系にした　20230511 by suzuki.tomoki

	// DIRECTXTKのメソッドは右手系　20230511 by suzuki.tomoki
	// 右手系にすると３角形頂点が反時計回りになるので描画されなくなるので注意
	// このコードは確認テストのために残す
	//	m_ViewMatrix = m_ViewMatrix.CreateLookAt(m_Position, m_Target, up);					

	Renderer::SetViewMatrix(&m_viewmtx);

	//プロジェクション行列の生成
	constexpr float fieldOfView = DirectX::XMConvertToRadians(45.0f);    // 視野角
	
	float aspectRatio = static_cast<float>(Application::GetWidth()) / static_cast<float>(Application::GetHeight());	// アスペクト比	
	float nearPlane = 1.0f;       // ニアクリップ
	float farPlane = 10000.0f;      // ファークリップ

	//プロジェクション行列の生成
	m_projmtx =
		DirectX::XMMatrixPerspectiveFovLH(
			fieldOfView, 
			aspectRatio, 
			nearPlane, 
			farPlane);	// 左手系にした　20230511 by suzuki.tomoki

	// DIRECTXTKのメソッドは右手系　20230511 by suzuki.tomoki
	// 右手系にすると３角形頂点が反時計回りになるので描画されなくなるので注意
	// このコードは確認テストのために残す
//	projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlane, farPlane);

	Renderer::SetProjectionMatrix(&m_projmtx);
}

Vector3 Camera::ScreenToWorld(float screenX, float screenY, float screenZ) const
{

	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(Application::GetWidth());
	vp.Height = static_cast<float>(Application::GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	XMVECTOR screenPos = XMVectorSet(screenX, screenY, screenZ, 1.0f);
	XMVECTOR worldPos = XMVector3Unproject(
		screenPos,
		vp.TopLeftX, vp.TopLeftY,
		vp.Width, vp.Height,
		vp.MinDepth, vp.MaxDepth,
		m_projmtx, m_viewmtx, XMMatrixIdentity()
	);

	Vector3 result;
	XMStoreFloat3(&result, worldPos);
	return result;

}
