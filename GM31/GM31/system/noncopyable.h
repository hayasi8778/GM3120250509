#pragma once

/**
 * @class NonCopyable
 * @brief �R�s�[���֎~���邽�߂̃N���X�ł��B
 *
 * ���̃N���X�́A�h���N���X�̃R�s�[�Ƒ����h�����߂ɐ݌v����Ă��܂��B
 * �R�s�[�R���X�g���N�^�ƃR�s�[������Z�q���폜���Ă��܂��B
 */
class NonCopyable {
public:
    /**
     * @brief �f�t�H���g�R���X�g���N�^�B
     */
    NonCopyable() = default;

    /**
     * @brief �R�s�[�R���X�g���N�^���폜�B
     * @details ���̃N���X�̃I�u�W�F�N�g�̃R�s�[�͋�����Ă��Ȃ����߁A�R�s�[�R���X�g���N�^�͍폜����Ă��܂��B
     */
    NonCopyable(const NonCopyable&) = delete;

    /**
     * @brief �R�s�[������Z�q���폜�B
     * @details ���̃N���X�̃I�u�W�F�N�g�̑���͋�����Ă��Ȃ����߁A�R�s�[������Z�q�͍폜����Ă��܂��B
     */
    NonCopyable& operator=(const NonCopyable&) = delete;
};
