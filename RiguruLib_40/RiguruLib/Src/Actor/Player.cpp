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

//�d��
#define GRABITY -10.0f
//�W�����v�̏����x
#define JUMPSPEED 15.0f
//�f�t�H���g�̃X�s�[�h
#define SPEED 3.0f
//�X�e�b�v�̏����x
#define STEPSPEED 25.0f
//�v���C���[�̔��a
#define RADIUS 0.5f;

Player::Player(IWorld& world_, std::shared_ptr<Stage> stage_) :Actor(world_), stage(stage_){
	//�p�����[�^�Z�b�g
	parameter.isDead = false;
	parameter.id = ACTOR_ID::PLAYER_ACTOR;
	parameter.cubeScale = vector3(1.0f, 3.0f, 1.0f);
	parameter.radius = RADIUS;
	parameter.matrix = RCMatrix4::Identity();
	pos = vector3(0.0f, 1.0f, 0);
	nor = vector3(0, 1, 0);
	speed = SPEED;
	drawMatrix = RCMatrix4::Identity();
	//��ԗp
	angleF = 0;
	grabity = vector3(0, 0, 0);

	//�e�X�g�`��p�̃t���O
	hitFlag = false;
	//�d�͂����邩�Ȃ���
	grabityFlag = false;

	//�A�j���[�V�����̏�����
	otherAnimID = ANIM_ID::WAIT_ANIM;
	animID = ANIM_ID::WAIT_ANIM;
	animCount = 0;
	otherAnimCount = animCount;
	animBlend = 0;

	//�W�����v�̏�����
	jumpFlag = false;
	jumpSpeed = JUMPSPEED;
	inertiaVec = vector3(0, 0, 0);
	jumpStartRotate = 0.0f;
	earlyLand = false;

	//�X�e�b�v�̏�����
	stepDelay = 0;
	stepFlag = false;

	//�ۂ܂�̏�����
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
	//�J�����̃p�����[�^���擾
	CAMERA_PARAMETER c = *Device::GetInstance().GetCamera()->CameraParam();
	//���͂��擾
	Vector3 vec = Control(frameTime, c);

	Matrix4 sizeMat, angleMat, tranMat;
	float s = 0.1f;
	//�T�C�Y�Z�b�g
	sizeMat = RCMatrix4::scale(vector3(s, s, s));
	//��]�̃}�g���b�N�X�Z�b�g
	angleMat = NorToRotateMat(c,nor);

	//�ړ��x�N�g����������
	parameter.moveVec = vector3(0, 0, 0);

	Vector3 moveVecFront = RCVector3::normalize(RCMatrix4::getFront(angleMat));
	Vector3 moveVecLeft = RCVector3::normalize(-RCMatrix4::getLeft(angleMat));

	//�ۂ܂蒆��������
	if (curlFlag){
		//�ۂ܂�O�̈ړ��ʂƃm�[�}������ǎC��x�N�g�������߂�
		Vector3 cl = (curlFront * curlInertiaVec.z + curlLeft * curlInertiaVec.x);
		cl = (cl - RCVector3::dot(cl, nor) * nor);
		//����𑫂�
		parameter.moveVec += cl * curlSpeed * frameTime;
		//�W�����v���Ɋۂ܂����Ȃ�
		if (jumpFlag){
			//�W�����v�̏����𑱂���
			Jump(frameTime);
			//�W�����v���̂����蔻��
			world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::SPHERE_MODEL_NATURAL_COLL);
		}
		else{
			//�x�߂̈ړ�
			//parameter.moveVec += (moveVecFront * vec.z + moveVecLeft * vec.x) * speed * frameTime;
			//�ǂ̖@�������������������Ȃ�
			if (nor.y > 0){
				//�v�Z�p�d��
				Vector3 g = vector3(0, GRABITY, 0);
				//�d�͂Ɩ@������ǎC��x�N�g�������ߑO��̂ɑ����Ċ��炩�ɉ������Ă���(0.75f�͖��C)
				wallVec += (g - RCVector3::dot(g, nor) * nor) * 0.75f * frameTime;
				//���̕ǎC��x�N�g������ǎC��x�N�g�������ߒ����n�ʂ�˂������ʂ悤�ɂ���
				Vector3 trueWall = wallVec - RCVector3::dot(wallVec, nor) * nor;
				//�ǎC��x�N�g�������Z����
				parameter.moveVec += trueWall * frameTime;
				//�n�ʂƂ̂����蔻��
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_NATURAL_COLL);
				//�ǂɂ߂荞�܂Ȃ��悤�ɂ���Step�p�̏���
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_STEP_COLL);
			}
			//�ǂ̖@���̌������������Ȃ�
			else{
				//jumpFlag = true;
				//�ǂ��痣�ꂽ��[
				if (!awayWallFlag){
					parameter.lastNor = nor;
				}
				awayWallFlag = true;
				//��]���̃x�N�g�����폜�i���A����������
				//curlSpeed = 0;
				jumpSpeed = JUMPSPEED;
				//moveVecUp = nor;
				grabityFlag = true;
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::SPHERE_MODEL_COLL);
			}
		}
	}
	//�ۂ܂蒆����Ȃ�������
	else{
		//�W�����v����������
		if (jumpFlag){
			//�����蔻����ۂ�
			world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::SPHERE_MODEL_NATURAL_COLL);
			//�W�����v���̏���
			Jump(frameTime);
		}
		//�W�����v������Ȃ�������
		else{
			//�����蔻����S�{�̃��C��
			world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_NATURAL_COLL);
		}
		//�X�e�b�v����������
		if (stepFlag){
			//�W�����v���̏ꍇ�͊ۂƃ��f���̂����蔻��ł����̂ł����ł̓W�����v������Ȃ��ꍇ��ݒ�
			if (!jumpFlag){
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_STEP_COLL);
			}
			//�X�e�b�v���̃X�s�[�h���Z�b�g
			speed = stepSpeed;
			//�^�������x�\��
			stepSpeed -= 65.0f * frameTime;
			//�~�܂�����߂�
			if (stepSpeed < 0){
				stepFlag = false;
				stepDelayFlag = true;
			}
			parameter.moveVec += (moveVecFront * stepVec.z + moveVecLeft * stepVec.x) * speed * frameTime;
		}
		else{
			//�X�e�b�v�X�s�[�h�̏�����
			stepSpeed = STEPSPEED;
			parameter.moveVec += (moveVecFront * vec.z + moveVecLeft * vec.x) * speed * frameTime;
		}
		//�X�e�b�v�̃f�B���C
		if (stepDelayFlag){
			//�f�B���C���Ԍv�Z
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
		//���͂���ړ��𑫂�(�d��)
		parameter.moveVec += grabity * frameTime;
	}
	else{
		//�e�X�g�p�㉺�ړ�
		pos -= RCVector3::normalize(RCMatrix4::getUp(parameter.matrix)) *  vec.y * frameTime * speed;
	}

	//�ړ�������
	pos += parameter.moveVec;
	//�g�����X�t�H�[���擾
	tranMat = RCMatrix4::translate(pos);
	//�}�g���b�N�X����
	parameter.matrix = sizeMat *angleMat * tranMat;


	//�`��p�ɕҏW
	{
		//�ۂ܂��Ă�����
		if (curlFlag){
			//�ǂ��痣��Ă��Ȃ����
			if (!awayWallFlag){
				Vector3 newLeft;
				if (!jumpFlag){
					//�i�s�����Əォ�烌�t�g������
					newLeft = RCVector3::cross(nor, RCVector3::normalize(parameter.moveVec));
				}
				else{
					newLeft = moveVecLeft;
				}
				//���̃��t�g�����ɉ�]������matrix�����
				angleMat = angleMat * RCQuaternion::rotate(newLeft, curlAngle);
				//�����ɉ����ĉ�]������
				curlAngle += RCVector3::length(parameter.moveVec) * 10000.0f * frameTime;
			}
			//������Ə�ɂ�����
			tranMat = RCMatrix4::translate(pos + (RCVector3::normalize(RCMatrix4::getUp(parameter.matrix)) * 0.1f) * curlFlag);
		}
	}

	//�`��p�}�g���b�N�X����
	drawMatrix = sizeMat * angleMat * tranMat;

	//�e�X�g�p
	{
		hitFlag = true/*�����ɏ����������ƕ\�������*/;
		std::string flagstring = hitFlag == true ? "TRUE" : "FALSE";
		Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 400), vector2(0.20f, 0.25f), 0.5f, "flag:" + flagstring);
		hitFlag = false;
	}
	//�d�͂��I��
	grabityFlag = true;
	//�J�����Ƀ}�g���b�N�X��n��
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

//�`��
void Player::Draw() const{
	//�v���C���[�p�̃V�F�[�_�[�Z�b�g
	Graphic::GetInstance().SetShader(SHADER_ID::PLAYER_SHADER);
	//�e�N�j�b�N�ƃp�X���Z�b�g
	Graphic::GetInstance().SetTechniquePass(SHADER_ID::PLAYER_SHADER, "TShader", "P0");

	//�A�j���[�V�������o�C���h
	Graphic::GetInstance().BindAnimation(animID, animCount, otherAnimID, otherAnimCount, animBlend);
	//�v���C���[��`��
	Graphic::GetInstance().DrawMesh(MODEL_ID::PLAYER_MODEL, &drawMatrix);

	//�e�X�g�t�H���g
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 585), vector2(0.20f, 0.25f), 0.5f, "PlayerPos.x:" + std::to_string(pos.x) + "f");
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 570), vector2(0.20f, 0.25f), 0.5f, "PlayerPos.y:" + std::to_string(pos.y) + "f");
	Graphic::GetInstance().DrawFont(FONT_ID::TEST_FONT, vector2(0, 555), vector2(0.20f, 0.25f), 0.5f, "PlayerPos.z:" + std::to_string(pos.z) + "f");
}

void Player::OnCollide(Actor& other, CollisionParameter colpara){
	hitFlag = true;
	//�����S�{�̃��C���������Ă�����
	if (colpara.id == COL_ID::RAY_MODEL_NATURAL_COLL){
		//���������ꏊ�ֈړ�
		pos = colpara.colPos;
		//�d�͂��I�t
		grabityFlag = false;
		//Up���Z�b�g
		nor = colpara.colNormal;
	}
	//�����ۂ��������Ă�����
	else if (colpara.id == COL_ID::SPHERE_MODEL_NATURAL_COLL){
		//�W�����v�̃X�s�[�h�������l��
		jumpSpeed = JUMPSPEED;
		//�W�����v����lerpCount�������l��
		jumpStartRotate = 0.0f;
		//�W�����v�t���O�I�t
		jumpFlag = false;
		//�X�e�b�v�t���O�I�t
		stepFlag = false;
		//Up���Z�b�g
		nor = colpara.colNormal;
		//���������ꏊ�ֈړ�
		pos = colpara.colPos;
		//�d�͂��I�t
		grabityFlag = false;
		//�����W�����v�̃A�j���[�V������葁�����n������
		if (otherAnimID == ANIM_ID::JUMP_ANIM){
			//�ύX���悤�Ƃ��Ă����A�j���[�V�������P�ɃZ�b�g
			animID = otherAnimID;
			//�ύX���悤�Ƃ��Ă����A�j���[�V�����̃J�E���g���ړ�
			animCount = otherAnimCount;
			//�ύX��̃A�j���[�V������Wait���Z�b�g
			otherAnimID = ANIM_ID::WAIT_ANIM;
			//Wait�̍Đ����Ԃ��O�ɃZ�b�g
			otherAnimCount = 0;
			//�t���O�𗧂Ă�
			earlyLand = true;
			//�u�����h�l���O��
			animBlend = 0;
		}
		//�ǂɂ�����[
		awayWallFlag = false;
	}
	//����Step�����C���ǂɂ߂荞�񂾂�
	else if (colpara.id == COL_ID::RAY_MODEL_STEP_COLL){ 
		pos = colpara.colPos;
	}
	//����Step�����C���ǂɂ߂荞�񂾂�
	else if (colpara.id == COL_ID::SPHERE_MODEL_COLL){
		pos = colpara.colPos;
		nor = colpara.colNormal;
		awayWallFlag = false;

		if (!Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LCTRL)){
			//�ۂ܂�I�t
			curlFlag = false;
			//�A�j���[�V�����ύX(����̂��߂Ɉ�ԏ�Ɏ����Ă����A���삪�Ȃ��Ȃ�Wait�ɑ��삪�������炻�̓����Ƀu�����h����)
			if (oldOtherAnimID != ANIM_ID::WAIT_ANIM){
				animID = oldOtherAnimID;
				otherAnimID = ANIM_ID::WAIT_ANIM;
				animBlend = 0;
			}
		}
	}
}

//����
Vector3 Player::Control(float frameTime, CAMERA_PARAMETER c){
	//�ړ��ʂ̏�����
	Vector3 vec = vector3(0, 0, 0);
	//���E�̓��͂����邩
	bool moveYawFlag = false;
	//�O��̓��͂����邩
	bool movePitchFlag = false;
	//�P�t���[���O�̃A�j���[�V������ۑ�
	oldAnimID = animID;
	oldOtherAnimID = otherAnimID;
	//�΂ߎ��̃A�j���[�V������������
	ANIM_ID DiagonallyAnimID = ANIM_ID::NULL_ANIM;

	//�ۂ܂�{�^���������ꂽ��
	if (Device::GetInstance().GetInput()->KeyUp(INPUTKEY::KEY_LCTRL) && !awayWallFlag){
		//�ۂ܂�I�t
		curlFlag = false;
		//�A�j���[�V�����ύX(����̂��߂Ɉ�ԏ�Ɏ����Ă����A���삪�Ȃ��Ȃ�Wait�ɑ��삪�������炻�̓����Ƀu�����h����)
		if (oldOtherAnimID != ANIM_ID::WAIT_ANIM){
			animID = oldOtherAnimID;
			otherAnimID = ANIM_ID::WAIT_ANIM;
			animBlend = 0;
		}
	}

	//����A�������ꂽ��
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_A)){
		//�x�N�g�����Z�b�g
		vec.x = -1;
		//�ۂ܂��Ă鎞�A�j���[�V�������o�O��Ȃ��悤��
		if (!curlFlag){
			//1�t���[���O�̃A�j���[�V������LEFT����Ȃ�������
			if (oldOtherAnimID != ANIM_ID::WARKLEFT_ANIM){
				//���܂��ɃW�����v������Ȃ�������
				if (!jumpFlag){
					//���ύX���悤�Ƃ��Ă����܂��͕ύX���I����Ă����A�j���[�V�������P�ɃZ�b�g
					animID = oldOtherAnimID;
					//�ύX���悤�Ƃ��Ă���A�j���[�V�����Z�b�g
					otherAnimID = ANIM_ID::WARKLEFT_ANIM;
					//�u�����h�l��������
					animBlend = 0;
				}
			}
			//���E�ɓ�������[
			moveYawFlag = true;
		}
	}
	//��̉E��
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
			//�������͂���Ă�����
			if (moveYawFlag){
				//�΂ߗp��ID���Z�b�g
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
	//�x�N�g���𐳋K��
	vec = RCVector3::normalize(vec);

	//�ۂ܂�̏����͂����� �Ƃ肠����������蓮���悤�ɂ͂ł���
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LCTRL)){
		if (!curlFlag){
			//�ۂ܂�I��
			curlFlag = true;
			//�X�e�b�v�����ǂ���
			if (stepFlag){
				//���̈ړ��x�N�g���������ɕۑ�
				curlInertiaVec = stepVec;
				//���̃X�s�[�h��ۑ�
				curlMaxSpeed = stepSpeed;
			}
			else {
				//���̈ړ��x�N�g���������ɕۑ�
				curlInertiaVec = vec;
				//���̃X�s�[�h��ۑ�
				curlMaxSpeed = speed;
			}

			//���̑O���Z�b�g
			curlFront = RCVector3::normalize(RCMatrix4::getFront(parameter.matrix));
			//���̍����Z�b�g
			curlLeft = RCVector3::normalize(-RCMatrix4::getLeft(parameter.matrix));

			//�ŏ��͂ǂ�����ꏏ
			curlSpeed = curlMaxSpeed; 

			//�ۂ܂莞�̉�]��������
			curlAngle = 0;

			//�X�e�b�v�̏������I��������
			stepFlag = false;
			stepDelay = false;

			//�ǎC��x�N�g��������
			wallVec = vector3(0, 0, 0);
			
			//�A�j���[�V�����ύX
			if (oldOtherAnimID != ANIM_ID::ROUND_ANIM){
				animID = oldOtherAnimID;
				otherAnimID = ANIM_ID::ROUND_ANIM;
				animBlend = 0;
			}
		}
		//���C(�N�����v��)
		if (curlSpeed > 0){
			curlSpeed -= curlMaxSpeed / 2 * frameTime;
		}
		else{
			curlSpeed = 0;
		}
		//����̃X�s�[�h��x������
		speed *= 0.5f;
	}


	//�ۂ܂蒆�͑����؂�̂Ńt���O�ň͂�
	if (!curlFlag){
		//�X�y�[�X�������ꂽ���W�����v������Ȃ�������
		if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_SPACE, true) && !jumpFlag){
			//����Z�b�g
			moveVecUp = nor;
			//�W�����v�t���O���I��
			jumpFlag = true;
			//������ۑ�
			inertiaVec = parameter.moveVec;

			//�A�j���[�V�����u�����h��������
			animBlend = 0;
			//���܂ł̃A�j���[�V�������P�ɃZ�b�g
			animID = oldOtherAnimID;
			//�J�E���g��
			animCount = otherAnimCount;
			//�W�����v�̃A�j���[�V�����͓r�����炪�]�܂�������
			otherAnimCount = 8;
			otherAnimID = ANIM_ID::JUMP_ANIM;
		}

		//�X�e�b�v�̏����͂������
		if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LSHIFT, true) && !stepFlag && !stepDelayFlag && (moveYawFlag || movePitchFlag)){
			stepFlag = true;
			stepVec = RCVector3::normalize(vec);
			if (!jumpFlag){
				world.SetCollideSelect(shared_from_this(), ACTOR_ID::STAGE_ACTOR, COL_ID::RAY_MODEL_STEP_COLL);
			}

		}

		if (!jumpFlag){
			//�΂߈ړ����A�j���[�V����
			if (moveYawFlag && movePitchFlag){
				animCount += 60.0f  * frameTime;
				otherAnimCount = animCount;
				animID = DiagonallyAnimID;
				//�΂ߗp��0.5f
				animBlend = 0.5f;
			}
			//�ʏ�ړ��A�j���[�V����
			else if (moveYawFlag || movePitchFlag){
				animCount += 60.0f  * frameTime;
				otherAnimCount = animCount;
				animBlend += 7.0f * frameTime;
			}
			else{
				if (animID == ANIM_ID::ROUND_ANIM){
					animBlend += 7.0f * frameTime;
				}
				//�ړ��������ɉ�]�����Ă�����(�E���)
				if (c.InputAngle.x > 0){
					//�E��]���Z�b�g
					if (oldOtherAnimID != ANIM_ID::TURNLEFT_ANIM){
						animID = oldOtherAnimID;
						animCount = otherAnimCount;
						otherAnimID = ANIM_ID::TURNLEFT_ANIM;
						animBlend = 0;
					}
					//�ő�l10.0f�ŃA�j���[�V�������Đ�
					animCount -= 6.0f  * frameTime * min(abs(c.InputAngle.x), 10.0f);
					otherAnimCount = animCount;
					animBlend += 7.0f * frameTime;
				}
				//�ړ��������ɉ�]�����Ă�����(�E���)
				else if (c.InputAngle.x < 0){
					if (oldOtherAnimID != ANIM_ID::TURNLEFT_ANIM){
						animID = oldOtherAnimID;
						animCount = otherAnimCount;
						otherAnimID = ANIM_ID::TURNLEFT_ANIM;
						animBlend = 0;
					}
					//�ő�l10.0f�ŃA�j���[�V�������Đ�
					animCount += 6.0f  * frameTime * min(abs(c.InputAngle.x), 10.0f);
					otherAnimCount = animCount;
					animBlend += 7.0f * frameTime;
				}
			}
		}
		else{
			//�W�����v���A�j���[�V����(��яオ���̗]�C)
			if (otherAnimCount > 16){
				animBlend += 7.0f * frameTime;
				if (animBlend > 1.0f){
					animCount = 16;
					otherAnimCount += 70.0f * frameTime;
				}
			}
			//�W�����v���A�j���[�V�����i��яオ��j
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


		//�������瑁�����n�����t���O���폜
		if (movePitchFlag || moveYawFlag || jumpFlag)earlyLand = false;
		//�������n���Ă�����
		if (earlyLand){
			//Wait�̃A�j���[�V�����ւ������ƕύX
			animBlend += 10.0f * frameTime;
			if (animBlend > 1.0f)earlyLand = false;
		}
	}
	//�ۂ܂�̃A�j���[�V�����Ɉڍs����ׂ̃u�����h�l���Z
	else{
		animBlend += 7.0f * frameTime;
	}

	//�e�X�g�p�̔�s�R�}���h
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_Z)){
		vec.y = -1;
	}
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_X)){
		vec.y = 1;
	}

	//���N���b�N���ꂽ�玅����
	if (Device::GetInstance().GetInput()->MouseButtonDownTrigger(INPUTMOUSEBUTTON::LEFT_BUTTON)){
		world.Add(ACTOR_ID::THREAD_ACTOR, std::make_shared<Thread>(world, shared_from_this(), stage));
	}

	//�{���̖ړI�𔼕��Y�ꂩ���������͂�Ԃ�
	return RCVector3::normalize(vec);
}

//�W�����v���̏����@�@
void Player::Jump(float frameTime){
	//�����x�̌v�Z
	jumpSpeed -= 25.0f * frameTime;
	if (jumpSpeed <= 0)jumpSpeed = 0.0f;
	//������Ɗ����𑫂��i�v�����j
	parameter.moveVec += (moveVecUp)* jumpSpeed * frameTime + RCVector3::normalize(inertiaVec) * jumpSpeed / 8.0f * frameTime;

	//���X��up��vector3(0,1,0)�ɋ߂Â���
	nor = RCVector3::lerp((moveVecUp), vector3(0, 1, 0), jumpStartRotate);
	jumpStartRotate += 6.0f * frameTime;
}

//�}�g���b�N�X��Ԃ�
Matrix4* Player::ReturnMat(){
	return &parameter.matrix;
}

//�m�[�}������Rotate���v�Z
Matrix4 Player::NorToRotateMat(CAMERA_PARAMETER c,Vector3 normal){
	Matrix4  angleMat;

	//�܂�Up������yaw�̉�]��������
	auto fm = RCQuaternion::rotate(normal, Math::angle(c.AngleH - angleF));
	//�����p�iplayer.h�݂āj
	angleF = c.AngleH;
	//���̃t�����g�ɂ������̉�]���|�������𖳎������O�����߂�
	auto dfv = RCMatrix4::getFront(parameter.matrix) * fm;

	//��ƍ����𖳎������O����O�ς�p���č������߂� �O�̕����������������Ă���Ɛ����Ȓ������ĕς��Ȃ���ł��A�������Ă�
	auto lv = RCVector3::cross(RCVector3::normalize(dfv), normal);
	//���������Əォ�琳�����O�����߂�
	auto fv = RCVector3::cross(normal, lv);

	//��]�s������߂�
	angleMat = RCMatrix4::Identity();
	RCMatrix4::setLeft(angleMat, RCVector3::normalize(-lv));
	RCMatrix4::setUp(angleMat, RCVector3::normalize(normal));
	RCMatrix4::setFront(angleMat, RCVector3::normalize(fv));

	return angleMat;
}