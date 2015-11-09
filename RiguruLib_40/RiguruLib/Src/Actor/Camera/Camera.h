#pragma once
#include "../../Other/DX11User.h"
#include "../../Math/Vector3.h"
#include "../../Math/Matrix4.h"
class Stage;
#include <memory>

// カメラのパラメータ構造体
struct CAMERA_PARAMETER
{
	float		AngleH;				// 水平角度
	float		AngleV;				// 垂直角度
	Vector3		Eye;				// カメラ座標
	Vector3		Target;				// 注視点座標
	Vector3		Up;					// 上方向
	Vector3		InputAngle;			// 入力
};

class Camera{
public:
	Camera(std::shared_ptr<Stage> stage_);
	~Camera();
	//ポジションとビューをセット
	void SetCamera(Vector3 cameraPos, Vector3 cameraView, float frameTime);
	//ビューのマトリックスを取得
	D3DXMATRIX returnView();
	//プロジェクションのマトリックスを取得
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