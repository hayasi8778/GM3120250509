#include "system/commonTypes.h"
#include "system/renderer.h"
#include "camera.h"
#include "application.h"
#include <DirectXMath.h>

using namespace DirectX;

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
	float farPlane = 1000.0f;      // ファークリップ

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
