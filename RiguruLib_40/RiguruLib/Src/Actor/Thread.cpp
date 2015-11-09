#include "Thread.h"
#include  "../Other/Device.h"
#include "../Math/Vector2.h"
#include "../Math/MathUtility.h"
#include "../Graphic/Graphic.h"
#include "Collision.h"
#include "ID.h"
#include "Player.h"
#include "Stage.h"

Thread::Thread(IWorld& world_, std::shared_ptr<Player> player_, std::shared_ptr<Stage> stage_) :Actor(world_), player(player_), stage(stage_){
	parameter.isDead = false;
	parameter.id = ACTOR_ID::THREAD_ACTOR;
	startPos = RCMatrix4::getPosition(*player->ReturnMat());
	Vector3 eye = Device::GetInstance().GetCamera()->CameraParam()->Eye;
	Vector3 target = Device::GetInstance().GetCamera()->CameraParam()->Target;
	Vector3 vec =RCVector3::normalize(target - eye);
	CollisionParameter col = ModelRay(*stage->ReturnMat(), OCT_ID::STAGE_OCT, eye,eye + vec * 10000.0f);
	CollisionParameter col_ = ModelRay(*stage->ReturnMat(), OCT_ID::STAGE_OCT, startPos + RCVector3::normalize(RCMatrix4::getUp(*player->ReturnMat())) * 0.1f, col.colPos);
	endPos = col_.colPos;
}

Thread::~Thread(){

}

void Thread::Update(float frameTime){
}

void Thread::Draw() const{
	Graphic::GetInstance().DrawLine(startPos, endPos);
}

void Thread::OnCollide(Actor& other, CollisionParameter colpara){
	//parameter.isDead = true;
}