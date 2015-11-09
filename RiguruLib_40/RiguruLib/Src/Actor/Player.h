#pragma once

#include "../Other/DX11User.h"
#include "../Other/D3D11User.h"
#include <map>
#include "ID.h"
#include "../Math/Matrix4.h"
#include "../Math/MatrixUtility.h"
#include "../Graphic/Rgr/MeshUser.h"
#include "Actor.h"
struct CAMERA_PARAMETER;
class Stage;
class Player :public Actor, public std::enable_shared_from_this<Player>{
public:
	Player(IWorld& world_, std::shared_ptr<Stage> stage_);
	~Player();
	virtual void Update(float frameTime) override;
	virtual void Draw() const override;
	virtual void OnCollide(Actor& other, CollisionParameter colpara) override;
	//�}�g���b�N�X��Ԃ�
	Matrix4* ReturnMat();
private:
	//����
	Vector3 Control(float frameTime,CAMERA_PARAMETER c);
	//�W�����v���̏���
	void Jump(float frameTime);
	//�m�[�}������Rotate���v�Z
	Matrix4 NorToRotateMat(CAMERA_PARAMETER c, Vector3 normal);
private:
	/********��{�p�����[�^********/
	//�X�e�[�W�̃|�C���^�AThread�ɑ���p
	std::shared_ptr<Stage> stage;
	//�e�X�g�\���p�̃t���O
	bool hitFlag;
	//�v���C���[�̌��X�s�[�h
	float speed;
	//�Q�[��������Ȃ��߂����蔻�蒆GURABITY��؂邽�߂̂���
	bool grabityFlag;
	//�v���C���[�̃|�W�V����
	Vector3 pos;
	//�@���ƌ����������ق�Up�ɒl����
	Vector3 nor;
	//�O�̉�]�ɑ����Ă����̂ō��������߂����
	float angleF;
	//�`��p�}�g���b�N�X
	Matrix4 drawMatrix;
	//�d��
	Vector3 grabity;

	/*************JUMP**************/
	//�W�����v���Ă��邩�ǂ���
	bool jumpFlag;
	//�A�j���[�V������葁�����n���Ă��邩�ǂ���
	bool earlyLand;
	//�W�����v�{�^�����������Ƃ��̏����
	Vector3 moveVecUp;
	//�W�����v�{�^�����������Ƃ��ɐi��ł�������
	Vector3 inertiaVec;
	//�W�����v���̌��X�s�[�h
	float jumpSpeed;
	//�W�����v����Up��vector3(0,1,0)�ɖ߂����Ɏg��Count
	float jumpStartRotate;

	/*************STEP***************/
	//�X�e�b�v���Ă��邩�ǂ���
	bool stepFlag;
	//�X�e�b�v�����Ă����莞�Ԍo���Ă��邩�ǂ���
	bool stepDelayFlag;
	//��莞�Ԃ��v�Z����Count
	float stepDelayCount;
	//�X�e�b�v�̃X�s�[�h
	float stepSpeed;
	//�X�e�b�v�{�^���������ꂽ���̕���
	Vector3 stepVec;
	//�f�B���C�̃J�E���g
	float stepDelay;

	/*************CURL***************/
	//�ۂ܂��Ă��邩�ǂ���
	bool curlFlag;
	//�ۂ܂�O�̊����̕���
	Vector3 curlInertiaVec;
	//�ۂ܂�O�̃X�s�[�h
	float curlMaxSpeed;
	//�ۂ܂�O�̃X�s�[�h(���C�̉e���󂯂Ă��)
	float curlSpeed;
	//�ǎC��x�N�g��
	Vector3 wallVec;
	//�ۂ܂�O��Front�̕���
	Vector3 curlFront;
	//�ۂ܂�O��Front�̕���
	Vector3 curlLeft;
	//�ۂ܂��Ă���Ƃ��̉�]
	float curlAngle;
	//�ǂƗ���Ă��邩
	bool awayWallFlag;
	//����Ă���Ƃ��̂t��
	Vector3 awayWallUp;
	//��̂��lerp�Ŏg��
	float awayWallUpLerpCount;

	/*************ANIM***************/
	//1�ڂ̃A�j���[�V�����J�E���g
	float animCount;
	//2�ڂ̃A�j���[�V�����J�E���g
	float otherAnimCount;
	//�A�j���[�V�����̃u�����h�l
	float animBlend;
	//1�ڂ̃A�j���[�V����ID
	ANIM_ID animID;
	//2�ڂ̃A�j���[�V����ID
	ANIM_ID otherAnimID;
	//1�ڂ�1�t���[���O�̃A�j���[�V����ID
	ANIM_ID oldAnimID;
	//2�ڂ�1�t���[���O�̃A�j���[�V����ID
	ANIM_ID oldOtherAnimID;
};