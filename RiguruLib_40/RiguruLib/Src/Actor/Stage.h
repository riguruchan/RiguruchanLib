#pragma once

#include "../Other/DX11User.h"
#include "../Other/D3D11User.h"
#include <map>
#include "../Math/Matrix4.h"
#include "../Math/MatrixUtility.h"
#include "../Graphic/Rgr/MeshUser.h"
#include "Actor.h"

class Stage :public Actor{
public:
	Stage(IWorld& world_);
	~Stage();
	virtual void Update(float frameTime) override;
	virtual void Draw() const override;
	virtual void OnCollide(Actor& other, CollisionParameter colpara) override;
	Matrix4* ReturnMat();
private:
	float angle = 0;
	float angleZ = 0;
	float bvolume = 50;
	float svolume = 50;
	float animCount = 0;
	float yAngle;
};