#pragma once
#include "../../Other/DX11User.h"
#include "../../Math/Vector3.h"
#include "../../Math/Matrix4.h"
class Stage;
#include <memory>

// �J�����̃p�����[�^�\����
struct CAMERA_PARAMETER
{
	float		AngleH;				// �����p�x
	float		AngleV;				// �����p�x
	Vector3		Eye;				// �J�������W
	Vector3		Target;				// �����_���W
	Vector3		Up;					// �����
	Vector3		InputAngle;			// ����
};

class Camera{
public:
	Camera(std::shared_ptr<Stage> stage_);
	~Camera();
	//�|�W�V�����ƃr���[���Z�b�g
	void SetCamera(Vector3 cameraPos, Vector3 cameraView, float frameTime);
	//�r���[�̃}�g���b�N�X���擾
	D3DXMATRIX returnView();
	//�v���W�F�N�V�����̃}�g���b�N�X���擾
	D3DXMATRIX returnProj();

	void SetMatrix(Matrix4 pos);

	CAMERA_PARAMETER* CameraParam();

	Matrix4 ReturnCameraRotate();

private:
	std::shared_ptr<Stage> stage;
	D3DXMATRIX matView, matProj;
	CAMERA_PARAMETER mCameraParam;
	Matrix4 mat;
	float angleC,angleF;
	Matrix4 matrix;
};