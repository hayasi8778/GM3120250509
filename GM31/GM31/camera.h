#pragma once

#include	"system/commontypes.h"
#include	"system/renderer.h"
#include	"application.h"
#include	"system/CPolar3D.h"

class Camera {
protected:
	Vector3	m_position = Vector3(0.0f, 0.0f, 0.0f);	// �J�����ʒu

	Vector3		m_lookat{};				// �����_
	Matrix4x4	m_viewmtx{};			// �r���[�s��
	Matrix4x4   m_projmtx{};			// �v���W�F�N�V�����s��

public:
	Camera() = default;

	void Init();
	void Dispose();
	void Update();
	void Draw();
	void SetPosition(const Vector3& position) { m_position = position; }
	void SetLookat(const Vector3& position) { m_lookat = position; }

	Matrix4x4 GetViewMatrix() const { return m_viewmtx; }
	Matrix4x4 GetProjMatrix() const { return m_projmtx; }
};

// / ���R���_�J����
class FreeCamera : public Camera {
private:
	float   m_elevation = -90.0f * PI / 180.0f;		// �p
	float	m_azimuth = PI / 2.0f;					// ���ʊp	
	float   m_radius = 100.0f;						// ���a	

	Vector3 m_up = { 0.0f, 1.0f, 0.0f };			// ������x�N�g��
public:
	void SetElevation(float elevation) {
		m_elevation = elevation;
	}

	void SetAzimuth(float azimuth) {
		m_azimuth = azimuth;
	}

	void SetRadius(float radius) 
{
		m_radius = radius;
	}

	void CalcCameraPosition() 
	{
		// �p�ƕ��ʊp����J�����̈ʒu���v�Z
		CPolor3D polar(m_radius, m_elevation, m_azimuth);
		// �ɍ��W����f�J���g���W�ɕϊ�
		m_position = polar.ToCartesian();

		CPolor3D polarup(m_radius, (m_elevation + (90.0f * PI / 180.0f)), m_azimuth);
		m_up = polarup.ToCartesian();
	}

	void Draw() {
		// �r���[�ϊ����쐬
		m_viewmtx =
			DirectX::XMMatrixLookAtLH(
				m_position,
				m_lookat,
				m_up);				// ����n�ɂ����@20230511 by suzuki.tomoki

		// DIRECTXTK�̃��\�b�h�͉E��n�@20230511 by suzuki.tomoki
		// �E��n�ɂ���ƂR�p�`���_�������v���ɂȂ�̂ŕ`�悳��Ȃ��Ȃ�̂Œ���
		// ���̃R�[�h�͊m�F�e�X�g�̂��߂Ɏc��
		//	m_ViewMatrix = m_ViewMatrix.CreateLookAt(m_Position, m_Target, up);					

		Renderer::SetViewMatrix(&m_viewmtx);

		//�v���W�F�N�V�����s��̐���
		constexpr float fieldOfView = DirectX::XMConvertToRadians(45.0f);    // ����p

		float aspectRatio = static_cast<float>(Application::GetWidth()) / static_cast<float>(Application::GetHeight());	// �A�X�y�N�g��	
		float nearPlane = 1.0f;       // �j�A�N���b�v
		float farPlane = 3000.0f;      // �t�@�[�N���b�v

		//�v���W�F�N�V�����s��̐���
		Matrix4x4 projectionMatrix;
		projectionMatrix =
			DirectX::XMMatrixPerspectiveFovLH(
				fieldOfView,
				aspectRatio,
				nearPlane,
				farPlane);	// ����n�ɂ����@20230511 by suzuki.tomoki

		// DIRECTXTK�̃��\�b�h�͉E��n�@20230511 by suzuki.tomoki
		// �E��n�ɂ���ƂR�p�`���_�������v���ɂȂ�̂ŕ`�悳��Ȃ��Ȃ�̂Œ���
		// ���̃R�[�h�͊m�F�e�X�g�̂��߂Ɏc��
	//	projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlane, farPlane);

		Renderer::SetProjectionMatrix(&projectionMatrix);

	}
};

