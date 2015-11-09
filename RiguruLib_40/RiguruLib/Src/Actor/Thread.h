#pragma once

#include "../Other/DX11User.h"
#include "../Other/D3D11User.h"
#include <map>
#include "../Math/Matrix4.h"
#include "../Math/MatrixUtility.h"
#include "../Graphic/Rgr/MeshUser.h"
#include "Actor.h"

class Stage;
class Player;

class Thread :public Actor{
public:
	Thread(IWorld& world_, std::shared_ptr<Player> player_, std::shared_ptr<Stage> stage_);
	~Thread();
	virtual void Update(float frameTime) override;
	virtual void Draw() const override;
	virtual void OnCollide(Actor& other,CollisionParameter colpara) override;

private:
	std::shared_ptr<Player> player;
	std::shared_ptr<Stage> stage;
	Matrix4 mouseMat;
	float angle = 0;
	float angleZ = 0;
	float bvolume = 50;
	float svolume = 50;
	float animCount = 0;
	Vector3 startPos;
	Vector3 endPos;
};