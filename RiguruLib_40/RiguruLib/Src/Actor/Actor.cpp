#include "Actor.h"
#include "../Math/Converter.h"
#include "../Graphic/Graphic.h"
#include "Collision.h"
#include <float.h>
Actor::Actor(IWorld& world_) :world(world_)
{
	cubeCount = 0;
	colFunc[COL_ID::SPHERE_SPHERE_COLL] = std::bind(&Actor::SphereSphere, this, std::placeholders::_1);
	colFunc[COL_ID::RAY_MODEL_COLL] = std::bind(&Actor::RayModel, this, std::placeholders::_1);
	colFunc[COL_ID::OBB_OBB_COLL] = std::bind(&Actor::OBBOBB, this, std::placeholders::_1);
	colFunc[COL_ID::RAY_MODEL_NATURAL_COLL] = std::bind(&Actor::RayModelNatural, this, std::placeholders::_1);
	colFunc[COL_ID::SPHERE_MODEL_COLL] = std::bind(&Actor::SphereModel, this, std::placeholders::_1);
	colFunc[COL_ID::SPHERE_MODEL_NATURAL_COLL] = std::bind(&Actor::SphereModelNatural, this, std::placeholders::_1);
	colFunc[COL_ID::CAPSULE_MODEL_COLL] = std::bind(&Actor::CapsuleModel, this, std::placeholders::_1);
	colFunc[COL_ID::RAY_MODEL_STEP_COLL] = std::bind(&Actor::RayModelStep, this, std::placeholders::_1);
	colFunc[COL_ID::RAY_MODEL_COLL] = std::bind(&Actor::RayModel, this, std::placeholders::_1);
}

Actor::~Actor(){

}

void Actor::Collide(COL_ID id, Actor& other){
	//CapsulevsCapsule();
	CollisionParameter colpara = colFunc[id](other);
	if (colpara.colFlag)
	{
		OnCollide(other, colpara);
		other.OnCollide(*this, colpara);
	}
}
//‹…‚Æ‹…‚Ì‚ ‚½‚è”»’è
CollisionParameter Actor::SphereSphere(const Actor& other) const{
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_R)){
		Graphic::GetInstance().DrawSphere(RCMatrix4::getPosition(parameter.matrix), parameter.radius);
		Graphic::GetInstance().DrawSphere(RCMatrix4::getPosition(other.parameter.matrix), other.parameter.radius);
	}
	CollisionParameter colpara;
	colpara.colPos = vector3(0, 0, 0);
	colpara.colNormal = vector3(0, 1, 0);
	if (RCVector3::distance_(RCMatrix4::getPosition(parameter.matrix), RCMatrix4::getPosition(other.parameter.matrix)) <=
		parameter.radius + other.parameter.radius)
		colpara.colFlag = true;
	else
		colpara.colFlag = false;
	return colpara;
}
//ƒŒƒC‚Æƒ‚ƒfƒ‹‚Ì‚ ‚½‚è”»’è
CollisionParameter Actor::RayModel(const Actor& other)const{
	Vector3 pos = RCMatrix4::getPosition(parameter.matrix);
	float rad = parameter.radius;
	Vector3 down = -RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 end = pos + down* rad;
	Graphic::GetInstance().DrawLine(pos, end);
	return ModelRay(other.parameter.matrix, other.parameter.octId, pos, end);
}
//ƒŒƒC‚Æƒ‚ƒfƒ‹‚Ì‚ ‚½‚è”»’è
CollisionParameter Actor::RayModelStep(const Actor& other)const{
	Vector3 pos = RCMatrix4::getPosition(parameter.matrix) + RCMatrix4::getUp(parameter.matrix) * 3.0f;
	Vector3 rad = RCVector3::normalize(parameter.moveVec);
	Vector3 start = pos + rad	* 0.5f;	
	Vector3 end = pos - rad		* 0.5f;
	Graphic::GetInstance().DrawLine(start, end);
	
	CollisionParameter colpara = ModelRay(other.parameter.matrix, other.parameter.octId, start, end);
	Vector3 nor = colpara.colNormal;
	if (!colpara.colFlag)return colpara;

	while (true){
		pos = pos - rad * 0.1f;
		start = pos + rad	* 0.5f;
		end = pos - rad		* 0.5f;
		colpara = ModelRay(other.parameter.matrix, other.parameter.octId, start, end);
		if (!colpara.colFlag){
			colpara.colFlag = true;
			colpara.id = COL_ID::RAY_MODEL_STEP_COLL;
			colpara.colPos = pos - rad * 0.3f;
			colpara.colNormal = nor;
			break;
		}
	}

	return colpara;
}
//ƒJƒvƒZƒ‹‚Æƒ‚ƒfƒ‹‚Ì‚ ‚½‚è”»’è
CollisionParameter Actor::CapsuleModel(const Actor& other)const{
	Vector3 down = -RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 pos = RCMatrix4::getPosition(parameter.matrix) - down;
	Vector3 end = down * 2.0f + pos;
	float len = RCVector3::length(end - pos);

	return ModelCapsule(other.parameter.matrix, other.parameter.octId, CreateCapsule(pos,end,parameter.radius));
}
//ƒŒƒC‚Æƒ‚ƒfƒ‹‚Ì‚ ‚½‚è”»’è(’wå”Å)
CollisionParameter Actor::RayModelNatural(const Actor& other)const{
	CollisionParameter colpara;
	colpara.colFlag = false;
	colpara.colPos = vector3(0, 0, 0);
	colpara.colNormal = vector3(0, 1, 0);
	float downVec = 2.0f;
	Vector3 down = -RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 up = RCVector3::normalize(RCMatrix4::getUp(parameter.matrix)) * 0.3f;
	Vector3 pos = RCMatrix4::getPosition(parameter.matrix) +up;
	Vector3 end = down * downVec + pos;
	float len = RCVector3::length(end - pos);
	colpara = ModelRay(other.parameter.matrix, other.parameter.octId, pos, end);

	if (colpara.colFlag){
	float zure = 0.2f;
	float rad =0.45f;
	Vector3 down1 = -RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 up1 = RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 pos1 = RCMatrix4::getPosition(parameter.matrix) + RCVector3::normalize(RCMatrix4::getLeft(parameter.matrix)) * 1 * rad + up;
	Vector3 end1 = pos1 + down1 * downVec + RCVector3::normalize(pos1 - pos) * zure;
	float len1 = RCVector3::length(end1 - pos);
	CollisionParameter colpara1 = ModelRay(other.parameter.matrix, other.parameter.octId, pos1, end1);

	Vector3 down2 = -RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 up2 = RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 pos2 = RCMatrix4::getPosition(parameter.matrix) - RCVector3::normalize(RCMatrix4::getLeft(parameter.matrix)) * 1 * rad + up;
	Vector3 end2 = pos2 + down2 * downVec + RCVector3::normalize(pos2 - pos) * zure;
	float len2 = RCVector3::length(end2 - pos);
	CollisionParameter colpara2 = ModelRay(other.parameter.matrix, other.parameter.octId, pos2, end2);

	Vector3 down3 = -RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 up3 = RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 pos3 = RCMatrix4::getPosition(parameter.matrix) + RCVector3::normalize(RCMatrix4::getFront(parameter.matrix)) * 1 * rad + up;
	Vector3 end3 = pos3 + down3 * downVec + RCVector3::normalize(pos3 - pos) * zure;
	float len3 = RCVector3::length(end3 - pos);
	CollisionParameter colpara3 = ModelRay(other.parameter.matrix, other.parameter.octId, pos3, end3);

	Vector3 down4 = -RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 up4 = RCVector3::normalize(RCMatrix4::getUp(parameter.matrix));
	Vector3 pos4 = RCMatrix4::getPosition(parameter.matrix) - RCVector3::normalize(RCMatrix4::getFront(parameter.matrix)) * 1 * rad + up;
	Vector3 end4 = pos4 + down4 * downVec + RCVector3::normalize(pos4 - pos) * zure;
	float len4 = RCVector3::length(end4 - pos);
	CollisionParameter colpara4 = ModelRay(other.parameter.matrix, other.parameter.octId, pos4, end4);

	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_R)){
		Graphic::GetInstance().DrawLine(pos, end);
		Graphic::GetInstance().DrawLine(pos1, end1);
		Graphic::GetInstance().DrawLine(pos2, end2);
		Graphic::GetInstance().DrawLine(pos3, end3);
		Graphic::GetInstance().DrawLine(pos4, end4);
	}

	if (!colpara1.colFlag || !colpara2.colFlag || !colpara3.colFlag || !colpara4.colFlag){
		colpara.id = COL_ID::RAY_MODEL_NATURAL_COLL;
			return colpara;
		}
		Vector3 centerLeft = RCVector3::lerp(colpara1.colPos, colpara2.colPos, 0.5f);
		Vector3 centerFront = RCVector3::lerp(colpara3.colPos, colpara4.colPos, 0.5f);

		Vector3 dummyHitPos;
		down = (down + RCMatrix4::getPosition(parameter.matrix));
		if (RCVector3::length(centerFront - down) >=
			RCVector3::length(centerLeft - down)){
			dummyHitPos = centerFront;
		}
		else dummyHitPos = centerLeft;
		if (RCVector3::length(colpara.colPos - down) >= RCVector3::length(dummyHitPos - down)){
			dummyHitPos = colpara.colPos;
		}
		colpara.colPos = dummyHitPos;
		Vector3 left = RCVector3::normalize(colpara1.colPos - colpara2.colPos);
		Vector3 front =RCVector3::normalize(colpara3.colPos - colpara4.colPos);

		colpara.colNormal = -RCVector3::cross(left, front);
	}
	colpara.id = COL_ID::RAY_MODEL_NATURAL_COLL;
	return colpara;
}
//‹…‚Æƒ‚ƒfƒ‹‚Ì‚ ‚½‚è”»’è
CollisionParameter Actor::SphereModel(const Actor& other)const{
	float r = RCVector3::length(parameter.moveVec) * 1.1f;
	Vector3 pos = RCMatrix4::getPosition(parameter.matrix) + RCVector3::normalize(RCMatrix4::getUp(parameter.matrix)) * r;
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LSHIFT))
	Graphic::GetInstance().DrawSphere(pos,r);
	CollisionParameter colpara = ModelSphere(other.parameter.matrix, other.parameter.octId, pos, r);
	colpara.id = COL_ID::SPHERE_MODEL_COLL;
	return colpara;
}
//‹…‚Æƒ‚ƒfƒ‹‚Ì‚ ‚½‚è”»’è(’wå”Å)
CollisionParameter Actor::SphereModelNatural(const Actor& other)const{
	float r = RCVector3::length(parameter.moveVec) * 1.1f;
	Vector3 pos = RCMatrix4::getPosition(parameter.matrix) + RCVector3::normalize(RCMatrix4::getUp(parameter.matrix)) * r;
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_LSHIFT))
	Graphic::GetInstance().DrawSphere(pos, r);
	CollisionParameter colpara = ModelSphere(other.parameter.matrix, other.parameter.octId, pos, r);
	colpara.id = COL_ID::SPHERE_MODEL_NATURAL_COLL;
	return colpara;
}
//‚n‚a‚a‚Æ‚n‚a‚a‚Ì‚ ‚½‚è”»’è
CollisionParameter Actor::OBBOBB(const Actor& other)const{
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_R)){
		Graphic::GetInstance().DrawCube(parameter.matrix, parameter.cubeScale);
		Graphic::GetInstance().DrawCube(other.parameter.matrix, other.parameter.cubeScale);
	}
	CollisionParameter colpara;
	OBB o1 = CreateOBB(parameter.matrix, parameter.cubeScale);
	OBB o2 = CreateOBB(other.parameter.matrix, other.parameter.cubeScale);
	colpara.colFlag = OBBvsOBB(o1, o2);
	colpara.colPos = vector3(0, 0, 0);
	colpara.colNormal = vector3(0, 1, 0);

	return colpara;
}