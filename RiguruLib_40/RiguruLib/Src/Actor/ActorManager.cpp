#include "ActorManager.h"
#include "Actor.h"
#include<algorithm>

ActorManager::ActorManager(){
}

ActorManager::~ActorManager(){

}

void ActorManager::Update(float frameTime){
	std::for_each(actorPtr.begin(), actorPtr.end(),
		[&](ActorPtr actor){ actor->Update(frameTime); });
}

void ActorManager::Draw() const{
	std::for_each(actorPtr.begin(), actorPtr.end(),
		[](ActorPtr actor){ actor->Draw(); });
}

void ActorManager::Add(ActorPtr actor){
	actorPtr.push_back(actor);
}

void ActorManager::Clear(){
	actorPtr.clear();
}
void ActorManager::Remove(){
	actorPtr.remove_if([](ActorPtr actor){return actor->IsDead(); });
}
void ActorManager::Collide(COL_ID id, Actor& other){
	std::for_each(actorPtr.begin(), actorPtr.end(),
		[&](ActorPtr actor){other.Collide(id, *actor); });
}
void ActorManager::Collide(COL_ID id, ActorManager& other){
	std::for_each(actorPtr.begin(), actorPtr.end(),
		[&](ActorPtr actor){other.Collide(id, *actor); });
}