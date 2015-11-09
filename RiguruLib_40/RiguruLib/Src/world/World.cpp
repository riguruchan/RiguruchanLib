#include "World.h"
#include "../actor/ID.h"

World::World(){

}

World::~World(){
	Clear();
}

void World::Initialize(){
	Clear();
}
void World::Update(float frameTime){
	actors.Update(frameTime);
}
void World::Draw() const{
	actors.Draw();
}
void World::Clear(){
	actors.Clear();
}

void World::Add(ACTOR_ID id, ActorPtr actor){
	actors.Add(id, actor);
}

void World::Add(ActorPtr actor){
	actors.Add(ACTOR_ID::PLAYER_ACTOR, actor);
}

bool World::IsEnd()const{
	//‚Æ‚è‚ ‚¦‚¸
	return false;
}

void World::SetCollideSelect(ActorPtr thisActor, ACTOR_ID otherID, COL_ID colID){
	actors.SetCollideSelect(thisActor, otherID, colID);
}