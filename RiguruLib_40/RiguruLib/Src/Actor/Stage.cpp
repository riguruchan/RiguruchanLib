#include "Stage.h"
#include  "../Other/Device.h"
#include "../Math/Vector2.h"
#include "../Math/MathUtility.h"
#include "../Graphic/Graphic.h"
#include "ID.h"
#include <string>
#include "Collision.h"

Stage::Stage(IWorld& world_):Actor(world_){
	parameter.isDead = false;
	parameter.id = ACTOR_ID::STAGE_ACTOR;
	parameter.cubeScale =vector3(10.0f,3.0f,10.0f);
	parameter.radius = 1.0f;
	yAngle = 0;
	parameter.octId = OCT_ID::STAGE_OCT;
	float size2 = 3.0f;
	parameter.matrix = RCMatrix4::matrix(
		vector3(size2, size2, size2),
		0.0f,
		yAngle,
		0.0f,
		vector3(-1, -2.0f, 0));
}

Stage::~Stage(){

}
void Stage::Update(float frameTime){

}

void Stage::Draw() const{
	Graphic::GetInstance().SetShader(SHADER_ID::STAGE_SHADER);
	Graphic::GetInstance().DrawMesh(MODEL_ID::STAGE_MODEL, &parameter.matrix,NULL,true);
}

void Stage::OnCollide(Actor& other, CollisionParameter colpara){

}

Matrix4* Stage::ReturnMat(){
	return &parameter.matrix;
}