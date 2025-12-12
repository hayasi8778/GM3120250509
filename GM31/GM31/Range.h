#pragma once
#include "system/commontypes.h"

//“¯—l‚Ì——R‚ÅGetrangeŠÖ”‚ğ“ü‚ê‚é
inline float GetRange(Vector3 vecA, Vector3 vecB) {
	Vector3 coppos_A = vecA;
	Vector3 coppos_B = vecB;
	if (coppos_A.x < 0) coppos_A.x *= -1;
	if (coppos_A.y < 0) coppos_A.y *= -1;
	if (coppos_A.z < 0) coppos_A.z *= -1;

	if (coppos_B.x < 0) coppos_B.x *= -1;
	if (coppos_B.y < 0) coppos_B.y *= -1;
	if (coppos_B.z < 0) coppos_B.z *= -1;
	Vector3 ranged = { coppos_A.x - coppos_B.x , coppos_A.y - coppos_B.y , coppos_A.z - coppos_B.z };
	if (ranged.x < 0) ranged.x *= -1;
	if (ranged.y < 0) ranged.y *= -1;
	if (ranged.z < 0) ranged.z *= -1;
	float rangedALL = ranged.x + ranged.y + ranged.z;

	return rangedALL;
}