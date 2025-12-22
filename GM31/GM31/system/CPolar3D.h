#pragma once
#include "commontypes.h"

#include "utility.h"
#include "ConeDrawer.h"

// ÇRÇcã…ç¿ïWån
class CPolor3D {
	float m_radius;				// îºåa		
	float m_elevation;			// ã¬äp
	float m_azimuth;			// ï˚à äp
	float m_hight;				// â~êçÇÃçÇÇ≥
public:
	CPolor3D() = delete;
	CPolor3D(float radius,
		float elevation,
		float azimuth) : m_radius(radius), m_elevation(elevation), m_azimuth(azimuth) {
	}
	CPolor3D(float radius,
		float elevation,
		float azimuth,
		float hight) : m_radius(radius), m_elevation(elevation), m_azimuth(azimuth), m_hight(hight) {}
	~CPolor3D() {}

	Vector3 ToCartesian() const{
		Vector3 position;

		//position.x = m_radius * sinf(m_elevation) * cosf(m_azimuth);
		//position.y = m_radius * cosf(m_elevation);
		//position.z = m_radius * sinf(m_elevation) * sinf(m_azimuth);
		position.x = m_radius * cosf(m_elevation) * sinf(m_azimuth);
		position.y = m_radius * sinf(m_elevation);
		position.z = m_radius * cosf(m_elevation) * cosf(m_azimuth);

		return position;
	}

	float GetRadius() { return m_radius; }
	float GetElevation() { return m_elevation; }
	float GetAzimuth() { return m_azimuth; }
	float GetHight() { return m_hight; }

};
