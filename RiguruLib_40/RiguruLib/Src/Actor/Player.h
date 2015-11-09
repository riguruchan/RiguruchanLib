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
	//マトリックスを返す
	Matrix4* ReturnMat();
private:
	//入力
	Vector3 Control(float frameTime,CAMERA_PARAMETER c);
	//ジャンプ中の処理
	void Jump(float frameTime);
	//ノーマルからRotateを計算
	Matrix4 NorToRotateMat(CAMERA_PARAMETER c, Vector3 normal);
private:
	/********基本パラメータ********/
	//ステージのポインタ、Threadに送る用
	std::shared_ptr<Stage> stage;
	//テスト表示用のフラグ
	bool hitFlag;
	//プレイヤーの現スピード
	float speed;
	//ゲームが特殊なためあたり判定中GURABITYを切るためのもの
	bool grabityFlag;
	//プレイヤーのポジション
	Vector3 pos;
	//法線と言いたいがほぼUpに値する
	Vector3 nor;
	//前の回転に足していくので差分を求めるもの
	float angleF;
	//描画用マトリックス
	Matrix4 drawMatrix;
	//重力
	Vector3 grabity;

	/*************JUMP**************/
	//ジャンプしているかどうか
	bool jumpFlag;
	//アニメーションより早く着地しているかどうか
	bool earlyLand;
	//ジャンプボタンを押したときの上方向
	Vector3 moveVecUp;
	//ジャンプボタンを押したときに進んでいた慣性
	Vector3 inertiaVec;
	//ジャンプ中の現スピード
	float jumpSpeed;
	//ジャンプ中にUpをvector3(0,1,0)に戻す時に使うCount
	float jumpStartRotate;

	/*************STEP***************/
	//ステップしているかどうか
	bool stepFlag;
	//ステップをしてから一定時間経っているかどうか
	bool stepDelayFlag;
	//一定時間を計算するCount
	float stepDelayCount;
	//ステップのスピード
	float stepSpeed;
	//ステップボタンが押された時の方向
	Vector3 stepVec;
	//ディレイのカウント
	float stepDelay;

	/*************CURL***************/
	//丸まっているかどうか
	bool curlFlag;
	//丸まる前の慣性の方向
	Vector3 curlInertiaVec;
	//丸まる前のスピード
	float curlMaxSpeed;
	//丸まる前のスピード(摩擦の影響受けてる方)
	float curlSpeed;
	//壁擦りベクトル
	Vector3 wallVec;
	//丸まる前のFrontの方向
	Vector3 curlFront;
	//丸まる前のFrontの方向
	Vector3 curlLeft;
	//丸まっているときの回転
	float curlAngle;
	//壁と離れているか
	bool awayWallFlag;
	//離れているときのＵｐ
	Vector3 awayWallUp;
	//上のやつのlerpで使う
	float awayWallUpLerpCount;

	/*************ANIM***************/
	//1つ目のアニメーションカウント
	float animCount;
	//2つ目のアニメーションカウント
	float otherAnimCount;
	//アニメーションのブレンド値
	float animBlend;
	//1つ目のアニメーションID
	ANIM_ID animID;
	//2つ目のアニメーションID
	ANIM_ID otherAnimID;
	//1つ目の1フレーム前のアニメーションID
	ANIM_ID oldAnimID;
	//2つ目の1フレーム前のアニメーションID
	ANIM_ID oldOtherAnimID;
};