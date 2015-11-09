#include "WorldActor.h"
#include<algorithm>
#include "../actor/ID.h"

WorldActor::WorldActor(){
	for (int i = ACTOR_ID::BEGIN_ACTOR; i <= ACTOR_ID::END_ACTOR; ++i)
		managers.emplace(ACTOR_ID(i), std::make_shared<ActorManager>());
}
WorldActor::~WorldActor(){

}
void WorldActor::Update(float frameTime){
	//全キャラアップデート
	std::for_each(managers.begin(), managers.end(),
		[&](ActorManagerPair pair){pair.second->Update(frameTime); });
	
	//あたり判定
	for (auto cols : colselect){
		for (auto sec : cols.second){
			managers[sec.otherID]->Collide(sec.colID, *cols.first);
		}
	}
	colselect.clear();

	//死んでるものを消す
	std::for_each(managers.begin(), managers.end(),
		[](ActorManagerPair pair){pair.second->Remove(); });
}
void WorldActor::Draw() const{
	//全キャラ描画
	std::for_each(managers.begin(), managers.end(),
		[](ActorManagerPair pair){pair.second->Draw(); });
}
void WorldActor::Add(ACTOR_ID id, ActorPtr actor){
	managers[id]->Add(actor);
}
void WorldActor::Clear(){
	//全キャラクリア
	std::for_each(managers.begin(), managers.end(),
		[](ActorManagerPair pair){pair.second->Clear(); });
}

void WorldActor::SetCollideSelect(ActorPtr thisActor, ACTOR_ID otherID, COL_ID colID){
	CollideSelect c = { otherID, colID };
	colselect[thisActor].push_back(c);
}