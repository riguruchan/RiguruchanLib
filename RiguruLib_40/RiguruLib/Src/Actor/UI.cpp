#include "UI.h"
#include  "../Other/Device.h"
#include "../Math/Vector2.h"
#include "../Math/MathUtility.h"
#include "../Graphic/Graphic.h"
#include "ID.h"
#include <string>
#include "Collision.h"

UI::UI(IWorld& world_):Actor(world_){
	parameter.isDead = false;
	parameter.id = ACTOR_ID::UI_ACTOR;
	parameter.cubeScale =vector3(10.0f,1.0f,1.0f);
	
}

UI::~UI(){

}

void UI::Update(float frameTime){
	float size2 = 1.0f;
	mouseMat = RCMatrix4::matrix(
		vector3(size2, size2, size2),
		0.0f,
		0.0f,
		0,
		vector3(-500,0,0));
}

void UI::Draw() const{
	Graphic::GetInstance().DrawTexture(TEXTURE_ID::FLOOR_TEXTURE, vector2(1024 / 2, 768 / 2), vector2(1.0f, 1.0f));
	Graphic::GetInstance().DrawTexture(TEXTURE_ID::CURSOR_TEXTURE, vector2(1024 / 2, 768 / 2), vector2(0.5f,0.5f));
}

void UI::OnCollide(Actor& other, CollisionParameter colpara){
}