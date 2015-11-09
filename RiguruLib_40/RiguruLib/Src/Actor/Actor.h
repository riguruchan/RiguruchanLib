#pragma once
#include "ID.h"
#include "../math/Matrix4.h"
#include "../math/Vector3.h"
#include "../math/Vector3Utility.h"
#include "../math/MatrixUtility.h"
#include "../Graphic/Rgr/OctreeUser.h"
#include "../Other/DX11User.h"
#include <functional>
#include "../world/IWorld.h"
//�����蔻��̃p�����[�^
struct CollisionParameter;
//�A�N�^�[�̃p�����[�^
struct ActorParameter
{
	ACTOR_ID id;
	Matrix4 matrix;
	bool isDead;
	Vector3 cubeScale;
	float radius;
	OCT_ID octId = OCT_ID::NULL_OCT;
	Vector3 moveVec;
	Vector3 lastNor;
};

class Actor{
public:
	Actor(IWorld& world_);
	~Actor();
	virtual void Update(float frameTime) = 0;
	virtual void Draw() const = 0;
	void Collide(COL_ID id, Actor& other);
	virtual bool IsDead() const{ return parameter.isDead; }
protected:
	virtual void OnCollide(Actor& other,CollisionParameter colpara) = 0;
	ActorParameter parameter;
	IWorld& world;

private:
	std::map<COL_ID, std::function<CollisionParameter(const Actor&)>> colFunc;
	//���Ƌ��̂����蔻��
	CollisionParameter SphereSphere(const Actor& other)const;
	//���C�ƃ��f���̂����蔻��
	CollisionParameter RayModel(const Actor& other)const;
	//���C�ƃ��f���̂����蔻��(�w偔ŃX�e�b�v)
	CollisionParameter RayModelStep(const Actor& other)const;
	//���C�ƃ��f���̂����蔻��(�w偔�)
	CollisionParameter RayModelNatural(const Actor& other)const;
	//���ƃ��f���̂����蔻��
	CollisionParameter SphereModel(const Actor& other)const;
	//���ƃ��f���̂����蔻��(�w偔�)
	CollisionParameter SphereModelNatural(const Actor& other)const;
	//�n�a�a�Ƃn�a�a�̂����蔻��
	CollisionParameter OBBOBB(const Actor& other)const;

	CollisionParameter CapsuleModel(const Actor& other)const;
	int cubeCount;
};