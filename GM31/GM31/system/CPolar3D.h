#pragma once
#include "commontypes.h"

// �R�c�ɍ��W�n
class CPolor3D {
	float m_radius;				// ���a		
	float m_elevation;			// �p
	float m_azimuth;			// ���ʊp
public:
	CPolor3D() = delete;
	CPolor3D(float radius,
		float elevation,
		float azimuth) : m_radius(radius), m_elevation(elevation), m_azimuth(azimuth) {}
	~CPolor3D() {}

	Vector3 ToCartesian() const{
		Vector3 position;

		position.x = m_radius * sinf(m_elevation) * cosf(m_azimuth);
		position.y = m_radius * cosf(m_elevation);
		position.z = m_radius * sinf(m_elevation) * sinf(m_azimuth);

		return position;
	}
};
