#include "Camera.h"
#include "../../Math/Converter.h"
#include "../../Other/Device.h"
#include ".././Collision.h"
#include "../../Graphic/Graphic.h"
#include "../Stage.h"
#define CAMERA_ANGLE_SPEED				0.005f * 60.0f		// ���񑬓x
#define CAMERA_PLAYER_TARGET_HEIGHT		1.0f		// �v���C���[���W����ǂꂾ�������ʒu�𒍎��_�Ƃ��邩
#define CAMERA_PLAYER_LENGTH			2		// �v���C���[�Ƃ̋���
#define CAMERA_COLLISION_SIZE			0.5f		// �J�����̓����蔻��T�C�Y

Camera::Camera(std::shared_ptr<Stage> stage_) :stage(stage_){
	mCameraParam.AngleH = 0;
	mCameraParam.AngleV = Math::radian(-45);
	mCameraParam.Eye = vector3(0, 0, 0);
	mCameraParam.Target = vector3(0, 0, 0);
	mCameraParam.Up = vector3(0, 1, 0);
	mat = RCMatrix4::Identity();
	angleC = 0;
	angleF = 0;
	matrix = RCMatrix4::Identity();
}
Camera::~Camera(){

}
//�|�W�V�����ƃr���[���Z�b�g
void Camera::SetCamera(Vector3 cameraPos, Vector3 cameraView,float frameTime){
	// �ˉe�s��
	D3DXMatrixPerspectiveFovLH(&matProj, 3.1415926f / 2.0f, 4.0f / 3.0f/*1.5f*/, 0.1f, 100.0f);
	mCameraParam.InputAngle = Device::GetInstance().GetInput()->MouseVec();
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_UP))
		mCameraParam.InputAngle.y = 1.0f;
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_DOWN))
		mCameraParam.InputAngle.y = -1.0f;
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_RIGHT))
		mCameraParam.InputAngle.x = 1.0f;
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LEFT))
		mCameraParam.InputAngle.x = -1.0f;
	//���͒l�����ƂɌv�Z
	mCameraParam.AngleH += mCameraParam.InputAngle.x * CAMERA_ANGLE_SPEED * frameTime;
	mCameraParam.AngleV += mCameraParam.InputAngle.y * CAMERA_ANGLE_SPEED * frameTime;
	//�N�����v
	if (mCameraParam.AngleH > Math::radian(180))mCameraParam.AngleH = Math::radian(-180);
	else if (mCameraParam.AngleH < Math::radian(-180))mCameraParam.AngleH = Math::radian(180);
	if (mCameraParam.AngleV > Math::radian(180))mCameraParam.AngleV = Math::radian(-180);
	else if (mCameraParam.AngleV < Math::radian(-180))mCameraParam.AngleV = Math::radian(180);

	//�e�X�g�t�H���g
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 485), vector2(0.20f, 0.25f), 0.5f,"AngleH:"+ std::to_string(Math::angle(mCameraParam.AngleH))+"f");
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 500), vector2(0.20f, 0.25f), 0.5f,"AngleV:"+ std::to_string(Math::angle(mCameraParam.AngleV))+"f");
	//�^�[�Q�b�g���v���C���[��菭����ɃZ�b�g
	mCameraParam.Target = RCMatrix4::getPosition(mat) + RCVector3::normalize(RCMatrix4::getUp(mat)) * CAMERA_PLAYER_TARGET_HEIGHT;//* Time::DeltaTime;

	//�^�[�Q�b�g�̈ʒu�̃}�g���b�N�X�擾
	Matrix4 t = RCMatrix4::translate(mCameraParam.Target);
	//�v���C���[�̍����g��
	Vector3 left = RCVector3::normalize(RCMatrix4::getLeft(mat));
	//�������ɏ㉺�̉�]�����߂�
	Matrix4 pitchMat = RCQuaternion::rotate(left, Math::angle(mCameraParam.AngleV - angleC));
	//�����p(Player.h�Q��)
	angleC = mCameraParam.AngleV;
	//���̑O�ɂ������̉�]���|����
	Vector3 front = RCVector3::normalize(RCMatrix4::getFront(matrix) * pitchMat);
	//�O�ς�p���ď�����߂�
	Vector3 up = RCVector3::normalize(RCVector3::cross(front, left));
	//�������O�����߂�
	front = RCVector3::normalize(RCVector3::cross(left, up));

	//���ɉ�����
	Matrix4 trans = RCMatrix4::translate(front * CAMERA_PLAYER_LENGTH);
	//Target�̈ʒu������ɉ�����
	Matrix4 x = t* trans;
	//�v���C���[�̂������ƃJ���������̂Ȃ��p
	float angle = Math::acos(RCVector3::dot(RCVector3::normalize(RCMatrix4::getFront(mat)), front));
	//��]�s����Z�b�g
	Matrix4 rotate = RCMatrix4::Identity();
	RCMatrix4::setLeft(rotate, left);
	RCMatrix4::setUp(rotate, up);
	RCMatrix4::setFront(rotate, front);
	matrix = rotate;

	//�v���C���[�̂������ƃJ���������̂Ȃ��p��70�x�ȏ�Ȃ�
	while (angle > 70.0f){
		//�P�x�������ċ��߂�
		if (RCVector3::cross(RCVector3::normalize(RCMatrix4::getFront(mat)), front) == left){
			mCameraParam.AngleV -= Math::radian(abs(angle) - 70.0f);
		}
		else{
			mCameraParam.AngleV += Math::radian(abs(angle) - 70.0f);
		}

		pitchMat = RCQuaternion::rotate(left, Math::angle(mCameraParam.AngleV - angleC));
		angleC = mCameraParam.AngleV;
		front = RCVector3::normalize(RCMatrix4::getFront(matrix) * pitchMat);
		up = RCVector3::normalize(RCVector3::cross(front, left));
		front = RCVector3::normalize(RCVector3::cross(left, up));
		angle = Math::acos(RCVector3::dot(RCVector3::normalize(RCMatrix4::getFront(mat)), front));
		rotate = RCMatrix4::Identity();
		RCMatrix4::setLeft(rotate, left);
		RCMatrix4::setUp(rotate, up);
		RCMatrix4::setFront(rotate, front);

		matrix = rotate;
		if (angle < 70.0f)break;
	}

	trans = RCMatrix4::translate(front * CAMERA_PLAYER_LENGTH);
	x = t* trans;
	mCameraParam.Eye = RCMatrix4::getPosition(x);//* Time::DeltaTime;
	
	//�J�����Ƃ̂����蔻��(�Q���T���@��p���ċ�����Z�����Ă���)
	bool hitNum = ModelCapsule(*stage->ReturnMat(), OCT_ID::STAGE_OCT, CreateCapsule(mCameraParam.Eye, mCameraParam.Target, CAMERA_COLLISION_SIZE)).colFlag;
	std::string s = (hitNum == true ? "TRUE" : "FALSE");
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 470), vector2(0.20f, 0.25f), 0.5f, "MODELCAPSULEFLAG:" +s );
	if (hitNum){
		float notHitLength = 0.0f;
		float cameraLen = CAMERA_PLAYER_LENGTH;
		do{
			float testLen = notHitLength + (cameraLen - notHitLength) / 2.0f;
			trans = RCMatrix4::translate(front * testLen);
			x = t* trans;
			mCameraParam.Eye = RCMatrix4::getPosition(x);//* Time::DeltaTime;
			bool hit = ModelCapsule(*stage->ReturnMat(), OCT_ID::STAGE_OCT, CreateCapsule(mCameraParam.Eye, mCameraParam.Target, CAMERA_COLLISION_SIZE)).colFlag;
			if (hit){
				cameraLen = testLen;
			}
			else{
				notHitLength = testLen;
			}
		} while (cameraLen - notHitLength > 0.01f);
	}

	mCameraParam.Up = up;//* Time::DeltaTime;
	// �r���[�s��
	D3DXMatrixLookAtLH(&matView,
		&RConvert(&mCameraParam.Eye),
		&RConvert(&mCameraParam.Target),
		&RConvert(&mCameraParam.Up));
}
//�r���[�̃}�g���b�N�X���擾
D3DXMATRIX Camera::returnView(){
	return matView;
}
//�v���W�F�N�V�����̃}�g���b�N�X���擾
D3DXMATRIX Camera::returnProj(){
	return matProj;
}

void Camera::SetMatrix(Matrix4 mat_){
	mat = mat_;
}

CAMERA_PARAMETER* Camera::CameraParam(){
	return &mCameraParam;
}

Matrix4 Camera::ReturnCameraRotate(){
	return mat;
}