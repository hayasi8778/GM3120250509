#pragma once
#include	"CommonTypes.h"

/**
 * @struct SRT
 * @brief �X�P�[���A��]�A���s�ړ��iTranslation�j�����i�[����\����
 */
struct SRT {
	Vector3 scale = { 1.0f,1.0f,1.0f };  ///< �X�P�[�����
	Vector3 rot = { 0.0f,0.0f,0.0f };    ///< ��]���
	Vector3 pos = { 0.0f,0.0f,0.0f };    ///< ���s�ړ��i�ʒu�j���

	Matrix4x4 GetMatrix() const {
		return Matrix4x4::CreateScale(scale) *
			Matrix4x4::CreateFromYawPitchRoll(rot.y, rot.x, rot.z) *
			Matrix4x4::CreateTranslation(pos);
	}
};
