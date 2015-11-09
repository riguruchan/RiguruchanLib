#include "Player.h"
#include  "../Other/Device.h"
#include "../Math/Vector2.h"
#include "../Math/MathUtility.h"
#include "../Graphic/Graphic.h"
#include "ID.h"
#include "../Audio/Audio.h"
#include "../Math/Converter.h"
#include "Collision.h"
#include "Thread.h"
#include "Stage.h"
#include "Camera\Camera.h"

//重力
#define GRABITY -10.0f
//ジャンプの初速度
#define JUMPSPEED 15.0f
//デフォルトのスピード
#define SPEED 3.0f
//ステップの初速度
#define STEPSPEED 25.0f
//プレイヤーの半径
#define RADIUS 0.5f;

Player::Player(IWorld& world_, std::shared_ptr<Stage> stage_) :Actor(world_), stage(stage_){
	//パラメータセット
	parameter.isDead = false;
	parameter.id = ACTOR_ID::PLAYER_ACTOR;
	parameter.cubeScale = vector3(1.0f, 3.0f, 1.0f);
	parameter.radius = RADIUS;
	parameter.matrix = RCMatrix4::Identity();
	pos = vector3(0.0f, 1.0f, 0);
	nor = vector3(0, 1, 0);
	speed = SPEED;
	drawMatrix = RCMatrix4::Identity();
	//補間用
	angleF = 0;
	grabity = vector3(0, 0, 0);

	//テスト描画用のフラグ
	hitFlag = false;
	//重力があるかないか
	grabityFlag = false;

	//アニメーションの初期化
	otherAnimID = ANIM_ID::WAIT_ANIM;
	animID = ANIM_ID::WAIT_ANIM;
	animCount = 0;
	otherAnimCount = animCount;
	animBlend = 0;

	//ジャンプの初期化
	jumpFlag = false;
	jumpSpeed = JUMPSPEED;
	inertiaVec = vector3(0, 0, 0);
	jumpStartRotate = 0.0f;
	earlyLand = false;

	//ステップの初期化
	stepDelay = 0;
	stepFlag = false;

	//丸まりの初期化
	curlFlag = false;
	curlInertiaVec = vector3(0, 0, 0);
	curlMaxSpeed = 0;
	curlSpeed = 0;
	wallVec = vector3(0, 0, 0);
	curlFront = vector3(0, 0, 0);
	curlLeft = vector3(0, 0, 0);
	curlAngle = 0;
	awayWallFlag = false;
	awayWallUp = vector3(0, 0, 0);
	awayWallUpLerpCount = 0;
}

Player::~Player(){
}
void Player::Update(float frameTime){
	parameter.radius = RADIUS;
	speed = SPEED;
	//カメラのパラメータを取得
	CAMERA_PARAMETER c = *Device::GetInstance().GetCamera()->CameraParam();
	//入力を取得
	Vector3 vec = Control(frameTime, c);

	Matrix4 sizeMat, angleMat, tranMat;
	float s = 0.1f;
	//サイズセット
	sizeMat = RCMatrix4::scale(vector3(s, s, s));
	//回転のマトリックスセット
	angleMat = NorToRotateMat(c,nor);

	//移動ベクトルを初期化
	parameter.moveVec = vector3(0, 0, 0);

	Vector3 moveVecFront = RCVector3::normalize(RCMatrix4::getFront(angleMat));
	Vector3 moveVecLeft = RCVector3::normalize(-RCMatrix4::getLeft(angleMat));

	//丸まり中だったら
	if (curlFlag){
		//丸まる前の移動量とノーマルから壁擦りベクトルを求める
		Vector3 cl = (curlFront * curlInertiaVec.z + curlLeft * curlInertiaVec.x);
		cl = (cl - RCVector3::dot(cl, nor) * nor);
		//それを足す
		parameter.moveVec += cl * curlSpeed * frameTime;
		//ジャンプ中に丸まったなら
		if (jumpFlag){
			//ジャンプの処理を続ける
			Jump(frameTime);
			//ジャンプ時のあたり判定
			world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::SPHERE_MODEL_NATURAL_COLL);
		}
		else{
			//遅めの移動
			//parameter.moveVec += (moveVecFront * vec.z + moveVecLeft * vec.x) * speed * frameTime;
			//壁の法線が横向きから上向きなら
			if (nor.y > 0){
				//計算用重力
				Vector3 g = vector3(0, GRABITY, 0);
				//重力と法線から壁擦りベクトルを求め前回のに足して滑らかに加速していく(0.75fは摩擦)
				wallVec += (g - RCVector3::dot(g, nor) * nor) * 0.75f * frameTime;
				//その壁擦りベクトルから壁擦りベクトルを求め直し地面を突き抜けぬようにする
				Vector3 trueWall = wallVec - RCVector3::dot(wallVec, nor) * nor;
				//壁擦りベクトルを加算する
				parameter.moveVec += trueWall * frameTime;
				//地面とのあたり判定
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_NATURAL_COLL);
				//壁にめり込まないようにするStep用の処理
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_STEP_COLL);
			}
			//壁の法線の向きが下向きなら
			else{
				//jumpFlag = true;
				//壁から離れたよー
				if (!awayWallFlag){
					parameter.lastNor = nor;
				}
				awayWallFlag = true;
				//回転中のベクトルを削除（リアル感落ちる
				//curlSpeed = 0;
				jumpSpeed = JUMPSPEED;
				//moveVecUp = nor;
				grabityFlag = true;
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::SPHERE_MODEL_COLL);
			}
		}
	}
	//丸まり中じゃなかったら
	else{
		//ジャンプ中だったら
		if (jumpFlag){
			//あたり判定を丸に
			world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::SPHERE_MODEL_NATURAL_COLL);
			//ジャンプ中の処理
			Jump(frameTime);
		}
		//ジャンプ中じゃなかったら
		else{
			//あたり判定を４本のレイに
			world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_NATURAL_COLL);
		}
		//ステップ中だったら
		if (stepFlag){
			//ジャンプ中の場合は丸とモデルのあたり判定でいいのでここではジャンプ中じゃない場合を設定
			if (!jumpFlag){
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_STEP_COLL);
			}
			//ステップ中のスピードをセット
			speed = stepSpeed;
			//疑似加速度表現
			stepSpeed -= 65.0f * frameTime;
			//止まったら戻す
			if (stepSpeed < 0){
				stepFlag = false;
				stepDelayFlag = true;
			}
			parameter.moveVec += (moveVecFront * stepVec.z + moveVecLeft * stepVec.x) * speed * frameTime;
		}
		else{
			//ステップスピードの初期化
			stepSpeed = STEPSPEED;
			parameter.moveVec += (moveVecFront * vec.z + moveVecLeft * vec.x) * speed * frameTime;
		}
		//ステップのディレイ
		if (stepDelayFlag){
			//ディレイ時間計算
			stepDelay += 1.0f * frameTime;
			if (stepDelay > 0.25f){
				stepDelay = 0;
				stepDelayFlag = false;
			}
		}
	}

	bool onGrabity = true;
	if (onGrabity){
		if (grabityFlag){
			grabity += vector3(0, GRABITY, 0) * 0.75f * frameTime;
		}
		else{
			grabity = vector3(0,0,0);
		}
		//入力から移動を足す(重力)
		parameter.moveVec += grabity * frameTime;
	}
	else{
		//テスト用上下移動
		pos -= RCVector3::normalize(RCMatrix4::getUp(parameter.matrix)) *  vec.y * frameTime * speed;
	}

	//移動させる
	pos += parameter.moveVec;
	//トランスフォーム取得
	tranMat = RCMatrix4::translate(pos);
	//マトリックス完成
	parameter.matrix = sizeMat *angleMat * tranMat;


	//描画用に編集
	{
		//丸まっていたら
		if (curlFlag){
			//壁から離れていなければ
			if (!awayWallFlag){
				Vector3 newLeft;
				if (!jumpFlag){
					//進行方向と上からレフトを求め
					newLeft = RCVector3::cross(nor, RCVector3::normalize(parameter.moveVec));
				}
				else{
					newLeft = moveVecLeft;
				}
				//そのレフトを軸に回転させるmatrixを作り
				angleMat = angleMat * RCQuaternion::rotate(newLeft, curlAngle);
				//長さに応じて回転させる
				curlAngle += RCVector3::length(parameter.moveVec) * 10000.0f * frameTime;
			}
			//ちょっと上にあげる
			tranMat = RCMatrix4::translate(pos + (RCVector3::normalize(RCMatrix4::getUp(parameter.matrix)) * 0.1f) * curlFlag);
		}
	}

	//描画用マトリックス完成
	drawMatrix = sizeMat * angleMat * tranMat;

	//テスト用
	{
		hitFlag = true/*ここに条件を書くと表示される*/;
		std::string flagstring = hitFlag == true ? "TRUE" : "FALSE";
		Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 400), vector2(0.20f, 0.25f), 0.5f, "flag:" + flagstring);
		hitFlag = false;
	}
	//重力をオン
	grabityFlag = true;
	//カメラにマトリックスを渡す
	Matrix4 cameraMatrix;
	if (!awayWallFlag){
		cameraMatrix = parameter.matrix;
		awayWallUpLerpCount = 0;
	}
	else{
		awayWallUp = RCVector3::lerp(nor, vector3(0, 1, 0), awayWallUpLerpCount);
		awayWallUpLerpCount += 6.0f * frameTime;
		cameraMatrix = sizeMat * NorToRotateMat(c,awayWallUp) * tranMat;
		float r = RADIUS;
		parameter.radius = r;
	}
	Device::GetInstance().GetCamera()->SetMatrix(cameraMatrix);
}

//描画
void Player::Draw() const{
	//プレイヤー用のシェーダーセット
	Graphic::GetInstance().SetShader(SHADER_ID::PLAYER_SHADER);
	//テクニックとパスをセット
	Graphic::GetInstance().SetTechniquePass(SHADER_ID::PLAYER_SHADER, "TShader", "P0");

	//アニメーションをバインド
	Graphic::GetInstance().BindAnimation(animID, animCount, otherAnimID, otherAnimCount, animBlend);
	//プレイヤーを描画
	Graphic::GetInstance().DrawMesh(MODEL_ID::PLAYER_MODEL, &drawMatrix);

	//テストフォント
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 585), vector2(0.20f, 0.25f), 0.5f, "PlayerPos.x:" + std::to_string(pos.x) + "f");
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 570), vector2(0.20f, 0.25f), 0.5f, "PlayerPos.y:" + std::to_string(pos.y) + "f");
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 555), vector2(0.20f, 0.25f), 0.5f, "PlayerPos.z:" + std::to_string(pos.z) + "f");
}

void Player::OnCollide(Actor& other, CollisionParameter colpara){
	hitFlag = true;
	//もし４本のレイが当たっていたら
	if (colpara.id == COL_ID::RAY_MODEL_NATURAL_COLL){
		//当たった場所へ移動
		pos = colpara.colPos;
		//重力をオフ
		grabityFlag = false;
		//Upをセット
		nor = colpara.colNormal;
	}
	//もし丸が当たっていたら
	else if (colpara.id == COL_ID::SPHERE_MODEL_NATURAL_COLL){
		//ジャンプのスピードを初期値に
		jumpSpeed = JUMPSPEED;
		//ジャンプ中のlerpCountを初期値に
		jumpStartRotate = 0.0f;
		//ジャンプフラグオフ
		jumpFlag = false;
		//ステップフラグオフ
		stepFlag = false;
		//Upをセット
		nor = colpara.colNormal;
		//当たった場所へ移動
		pos = colpara.colPos;
		//重力をオフ
		grabityFlag = false;
		//もしジャンプのアニメーションより早く着地したら
		if (otherAnimID == ANIM_ID::JUMP_ANIM){
			//変更しようとしていたアニメーションを１にセット
			animID = otherAnimID;
			//変更しようとしていたアニメーションのカウントを移動
			animCount = otherAnimCount;
			//変更後のアニメーションにWaitをセット
			otherAnimID = ANIM_ID::WAIT_ANIM;
			//Waitの再生時間を０にセット
			otherAnimCount = 0;
			//フラグを立てる
			earlyLand = true;
			//ブレンド値を０に
			animBlend = 0;
		}
		//壁についたよー
		awayWallFlag = false;
	}
	//もしStep中レイが壁にめり込んだら
	else if (colpara.id == COL_ID::RAY_MODEL_STEP_COLL){ 
		pos = colpara.colPos;
	}
	//もしStep中レイが壁にめり込んだら
	else if (colpara.id == COL_ID::SPHERE_MODEL_COLL){
		pos = colpara.colPos;
		nor = colpara.colNormal;
		awayWallFlag = false;

		if (!Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LCTRL)){
			//丸まりオフ
			curlFlag = false;
			//アニメーション変更(これのために一番上に持ってきた、操作がないならWaitに操作があったらその動きにブレンドする)
			if (oldOtherAnimID != ANIM_ID::WAIT_ANIM){
				animID = oldOtherAnimID;
				otherAnimID = ANIM_ID::WAIT_ANIM;
				animBlend = 0;
			}
		}
	}
}

//入力
Vector3 Player::Control(float frameTime, CAMERA_PARAMETER c){
	//移動量の初期化
	Vector3 vec = vector3(0, 0, 0);
	//左右の入力があるか
	bool moveYawFlag = false;
	//前後の入力があるか
	bool movePitchFlag = false;
	//１フレーム前のアニメーションを保存
	oldAnimID = animID;
	oldOtherAnimID = otherAnimID;
	//斜め時のアニメーションを初期化
	ANIM_ID DiagonallyAnimID = ANIM_ID::NULL_ANIM;

	//丸まりボタンが離されたら
	if (Device::GetInstance().GetInput()->KeyUp(INPUTKEY::KEY_LCTRL) && !awayWallFlag){
		//丸まりオフ
		curlFlag = false;
		//アニメーション変更(これのために一番上に持ってきた、操作がないならWaitに操作があったらその動きにブレンドする)
		if (oldOtherAnimID != ANIM_ID::WAIT_ANIM){
			animID = oldOtherAnimID;
			otherAnimID = ANIM_ID::WAIT_ANIM;
			animBlend = 0;
		}
	}

	//もしAが押されたら
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_A)){
		//ベクトルをセット
		vec.x = -1;
		//丸まってる時アニメーションがバグらないように
		if (!curlFlag){
			//1フレーム前のアニメーションがLEFTじゃなかったら
			if (oldOtherAnimID != ANIM_ID::WARKLEFT_ANIM){
				//おまけにジャンプ中じゃなかったら
				if (!jumpFlag){
					//今変更しようとしていたまたは変更が終わっていたアニメーションを１にセット
					animID = oldOtherAnimID;
					//変更しようとしているアニメーションセット
					otherAnimID = ANIM_ID::WARKLEFT_ANIM;
					//ブレンド値を初期化
					animBlend = 0;
				}
			}
			//左右に動いたよー
			moveYawFlag = true;
		}
	}
	//上の右版
	else if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_D)){
		vec.x = 1;
		if (!curlFlag){
			if (oldOtherAnimID != ANIM_ID::WARKRIGHT_ANIM){
				if (!jumpFlag){
					animID = oldOtherAnimID;
					otherAnimID = ANIM_ID::WARKRIGHT_ANIM;
					animBlend = 0;
				}
			}
			moveYawFlag = true;
		}
	}

	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_W)){
		vec.z = -1;

		if (!jumpFlag && !curlFlag){
			//横が入力されていたら
			if (moveYawFlag){
				//斜め用のIDをセット
				DiagonallyAnimID = ANIM_ID::WARKFRONT_ANIM;
				animBlend = 0;
			}
			else if (oldOtherAnimID != ANIM_ID::WARKFRONT_ANIM){
				animID = oldOtherAnimID;
				otherAnimID = ANIM_ID::WARKFRONT_ANIM;
				animBlend = 0;
			}

		}
		movePitchFlag = true;

	}
	else if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_S)){
		vec.z = 1;
		if (!jumpFlag && !curlFlag){
			if (moveYawFlag){
				DiagonallyAnimID = ANIM_ID::WARKBACK_ANIM;
				animBlend = 0;
			}
			else if (oldOtherAnimID != ANIM_ID::WARKBACK_ANIM){
				animID = oldOtherAnimID;
				otherAnimID = ANIM_ID::WARKBACK_ANIM;
				animBlend = 0;
			}
		}
		movePitchFlag = true;

	}
	//ベクトルを正規化
	vec = RCVector3::normalize(vec);

	//丸まりの処理はこちら とりあえずゆっくり動くようにはできた
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LCTRL)){
		if (!curlFlag){
			//丸まりオン
			curlFlag = true;
			//ステップ中かどうか
			if (stepFlag){
				//今の移動ベクトルを慣性に保存
				curlInertiaVec = stepVec;
				//今のスピードを保存
				curlMaxSpeed = stepSpeed;
			}
			else {
				//今の移動ベクトルを慣性に保存
				curlInertiaVec = vec;
				//今のスピードを保存
				curlMaxSpeed = speed;
			}

			//今の前をセット
			curlFront = RCVector3::normalize(RCMatrix4::getFront(parameter.matrix));
			//今の左をセット
			curlLeft = RCVector3::normalize(-RCMatrix4::getLeft(parameter.matrix));

			//最初はどちらも一緒
			curlSpeed = curlMaxSpeed; 

			//丸まり時の回転を初期化
			curlAngle = 0;

			//ステップの処理を終了させる
			stepFlag = false;
			stepDelay = false;

			//壁擦りベクトル初期化
			wallVec = vector3(0, 0, 0);
			
			//アニメーション変更
			if (oldOtherAnimID != ANIM_ID::ROUND_ANIM){
				animID = oldOtherAnimID;
				otherAnimID = ANIM_ID::ROUND_ANIM;
				animBlend = 0;
			}
		}
		//摩擦(クランプ含)
		if (curlSpeed > 0){
			curlSpeed -= curlMaxSpeed / 2 * frameTime;
		}
		else{
			curlSpeed = 0;
		}
		//操作のスピードを遅くする
		speed *= 0.5f;
	}


	//丸まり中は操作を切るのでフラグで囲う
	if (!curlFlag){
		//スペースが押されたかつジャンプ中じゃなかったら
		if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_SPACE, true) && !jumpFlag){
			//上をセット
			moveVecUp = nor;
			//ジャンプフラグをオン
			jumpFlag = true;
			//慣性を保存
			inertiaVec = parameter.moveVec;

			//アニメーションブレンドを初期化
			animBlend = 0;
			//今までのアニメーションを１にセット
			animID = oldOtherAnimID;
			//カウントも
			animCount = otherAnimCount;
			//ジャンプのアニメーションは途中からが望ましかった
			otherAnimCount = 8;
			otherAnimID = ANIM_ID::JUMP_ANIM;
		}

		//ステップの処理はこちらへ
		if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LSHIFT, true) && !stepFlag && !stepDelayFlag && (moveYawFlag || movePitchFlag)){
			stepFlag = true;
			stepVec = RCVector3::normalize(vec);
			if (!jumpFlag){
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_STEP_COLL);
			}

		}

		if (!jumpFlag){
			//斜め移動中アニメーション
			if (moveYawFlag && movePitchFlag){
				animCount += 60.0f  * frameTime;
				otherAnimCount = animCount;
				animID = DiagonallyAnimID;
				//斜め用に0.5f
				animBlend = 0.5f;
			}
			//通常移動アニメーション
			else if (moveYawFlag || movePitchFlag){
				animCount += 60.0f  * frameTime;
				otherAnimCount = animCount;
				animBlend += 7.0f * frameTime;
			}
			else{
				if (animID == ANIM_ID::ROUND_ANIM){
					animBlend += 7.0f * frameTime;
				}
				//移動をせずに回転をしていたら(右回り)
				if (c.InputAngle.x > 0){
					//右回転をセット
					if (oldOtherAnimID != ANIM_ID::TURNLEFT_ANIM){
						animID = oldOtherAnimID;
						animCount = otherAnimCount;
						otherAnimID = ANIM_ID::TURNLEFT_ANIM;
						animBlend = 0;
					}
					//最大値10.0fでアニメーションを再生
					animCount -= 6.0f  * frameTime * min(abs(c.InputAngle.x), 10.0f);
					otherAnimCount = animCount;
					animBlend += 7.0f * frameTime;
				}
				//移動をせずに回転をしていたら(右回り)
				else if (c.InputAngle.x < 0){
					if (oldOtherAnimID != ANIM_ID::TURNLEFT_ANIM){
						animID = oldOtherAnimID;
						animCount = otherAnimCount;
						otherAnimID = ANIM_ID::TURNLEFT_ANIM;
						animBlend = 0;
					}
					//最大値10.0fでアニメーションを再生
					animCount += 6.0f  * frameTime * min(abs(c.InputAngle.x), 10.0f);
					otherAnimCount = animCount;
					animBlend += 7.0f * frameTime;
				}
			}
		}
		else{
			//ジャンプ中アニメーション(飛び上がり後の余韻)
			if (otherAnimCount > 16){
				animBlend += 7.0f * frameTime;
				if (animBlend > 1.0f){
					animCount = 16;
					otherAnimCount += 70.0f * frameTime;
				}
			}
			//ジャンプ中アニメーション（飛び上がり）
			else{
				animBlend += 7.0f * frameTime;
				animCount += 60.0f  * frameTime;
				otherAnimCount += 30.0f * frameTime;
				if (otherAnimCount > 16){
					animBlend = 0;
					animID = ANIM_ID::JUMP_ANIM;
					animCount = otherAnimCount;
					otherAnimID = ANIM_ID::DURINGJUMP_ANIM;
				}
			}
		}


		//動いたら早く着地したフラグを削除
		if (movePitchFlag || moveYawFlag || jumpFlag)earlyLand = false;
		//早く着地していたら
		if (earlyLand){
			//Waitのアニメーションへさっさと変更
			animBlend += 10.0f * frameTime;
			if (animBlend > 1.0f)earlyLand = false;
		}
	}
	//丸まりのアニメーションに移行する為のブレンド値加算
	else{
		animBlend += 7.0f * frameTime;
	}

	//テスト用の飛行コマンド
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_Z)){
		vec.y = -1;
	}
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_X)){
		vec.y = 1;
	}

	//左クリックされたら糸発射
	if (Device::GetInstance().GetInput()->MouseButtonDownTrigger(INPUTMOUSEBUTTON::LEFT_BUTTON)){
		world.Add(ACTOR_ID::THREAD_ACTOR, std::make_shared<Thread>(world, shared_from_this(), stage));
	}

	//本来の目的を半分忘れかけたが入力を返す
	return RCVector3::normalize(vec);
}

//ジャンプ中の処理　　
void Player::Jump(float frameTime){
	//加速度の計算
	jumpSpeed -= 25.0f * frameTime;
	if (jumpSpeed <= 0)jumpSpeed = 0.0f;
	//上方向と慣性を足す（要調整）
	parameter.moveVec += (moveVecUp)* jumpSpeed * frameTime + RCVector3::normalize(inertiaVec) * jumpSpeed / 8.0f * frameTime;

	//徐々にupをvector3(0,1,0)に近づける
	nor = RCVector3::lerp((moveVecUp), vector3(0, 1, 0), jumpStartRotate);
	jumpStartRotate += 6.0f * frameTime;
}

//マトリックスを返す
Matrix4* Player::ReturnMat(){
	return &parameter.matrix;
}

//ノーマルからRotateを計算
Matrix4 Player::NorToRotateMat(CAMERA_PARAMETER c,Vector3 normal){
	Matrix4  angleMat;

	//まずUpを軸にyawの回転をさせる
	auto fm = RCQuaternion::rotate(normal, Math::angle(c.AngleH - angleF));
	//差分用（player.hみて）
	angleF = c.AngleH;
	//今のフロントにさっきの回転を掛け高さを無視した前を求める
	auto dfv = RCMatrix4::getFront(parameter.matrix) * fm;

	//上と高さを無視した前から外積を用いて左を求める 前の方向が高さ無視しても上と垂直な直線って変わらないんです、理解してね
	auto lv = RCVector3::cross(RCVector3::normalize(dfv), normal);
	//正しい左と上から正しい前を求める
	auto fv = RCVector3::cross(normal, lv);

	//回転行列を求める
	angleMat = RCMatrix4::Identity();
	RCMatrix4::setLeft(angleMat, RCVector3::normalize(-lv));
	RCMatrix4::setUp(angleMat, RCVector3::normalize(normal));
	RCMatrix4::setFront(angleMat, RCVector3::normalize(fv));

	return angleMat;
}