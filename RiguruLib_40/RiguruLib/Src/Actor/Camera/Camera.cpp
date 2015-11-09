#include "Camera.h"
#include "../../Math/Converter.h"
#include "../../Other/Device.h"
#include ".././Collision.h"
#include "../../Graphic/Graphic.h"
#include "../Stage.h"
#define CAMERA_ANGLE_SPEED				0.005f * 60.0f		// 旋回速度
#define CAMERA_PLAYER_TARGET_HEIGHT		1.0f		// プレイヤー座標からどれだけ高い位置を注視点とするか
#define CAMERA_PLAYER_LENGTH			2		// プレイヤーとの距離
#define CAMERA_COLLISION_SIZE			0.5f		// カメラの当たり判定サイズ

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
//ポジションとビューをセット
void Camera::SetCamera(Vector3 cameraPos, Vector3 cameraView,float frameTime){
	// 射影行列
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
	//入力値をもとに計算
	mCameraParam.AngleH += mCameraParam.InputAngle.x * CAMERA_ANGLE_SPEED * frameTime;
	mCameraParam.AngleV += mCameraParam.InputAngle.y * CAMERA_ANGLE_SPEED * frameTime;
	//クランプ
	if (mCameraParam.AngleH > Math::radian(180))mCameraParam.AngleH = Math::radian(-180);
	else if (mCameraParam.AngleH < Math::radian(-180))mCameraParam.AngleH = Math::radian(180);
	if (mCameraParam.AngleV > Math::radian(180))mCameraParam.AngleV = Math::radian(-180);
	else if (mCameraParam.AngleV < Math::radian(-180))mCameraParam.AngleV = Math::radian(180);

	//テストフォント
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 485), vector2(0.20f, 0.25f), 0.5f,"AngleH:"+ std::to_string(Math::angle(mCameraParam.AngleH))+"f");
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 500), vector2(0.20f, 0.25f), 0.5f,"AngleV:"+ std::to_string(Math::angle(mCameraParam.AngleV))+"f");
	//ターゲットをプレイヤーより少し上にセット
	mCameraParam.Target = RCMatrix4::getPosition(mat) + RCVector3::normalize(RCMatrix4::getUp(mat)) * CAMERA_PLAYER_TARGET_HEIGHT;//* Time::DeltaTime;

	//ターゲットの位置のマトリックス取得
	Matrix4 t = RCMatrix4::translate(mCameraParam.Target);
	//プレイヤーの左を使う
	Vector3 left = RCVector3::normalize(RCMatrix4::getLeft(mat));
	//左を軸に上下の回転を求める
	Matrix4 pitchMat = RCQuaternion::rotate(left, Math::angle(mCameraParam.AngleV - angleC));
	//差分用(Player.h参照)
	angleC = mCameraParam.AngleV;
	//今の前にさっきの回転を掛ける
	Vector3 front = RCVector3::normalize(RCMatrix4::getFront(matrix) * pitchMat);
	//外積を用いて上を求める
	Vector3 up = RCVector3::normalize(RCVector3::cross(front, left));
	//正しい前を求める
	front = RCVector3::normalize(RCVector3::cross(left, up));

	//後ろに下げる
	Matrix4 trans = RCMatrix4::translate(front * CAMERA_PLAYER_LENGTH);
	//Targetの位置から後ろに下げる
	Matrix4 x = t* trans;
	//プレイヤーのけつ方向とカメラ方向のなす角
	float angle = Math::acos(RCVector3::dot(RCVector3::normalize(RCMatrix4::getFront(mat)), front));
	//回転行列をセット
	Matrix4 rotate = RCMatrix4::Identity();
	RCMatrix4::setLeft(rotate, left);
	RCMatrix4::setUp(rotate, up);
	RCMatrix4::setFront(rotate, front);
	matrix = rotate;

	//プレイヤーのけつ方向とカメラ方向のなす角が70度以上なら
	while (angle > 70.0f){
		//１度ずつ引いて求める
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
	
	//カメラとのあたり判定(２分探索法を用いて距離を短くしていく)
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
	// ビュー行列
	D3DXMatrixLookAtLH(&matView,
		&RConvert(&mCameraParam.Eye),
		&RConvert(&mCameraParam.Target),
		&RConvert(&mCameraParam.Up));
}
//ビューのマトリックスを取得
D3DXMATRIX Camera::returnView(){
	return matView;
}
//プロジェクションのマトリックスを取得
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