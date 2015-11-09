#pragma once
#include "../actor/ActorPtr.h"
#include "../actor/ActorManager.h"
#include <map>
#include <vector>

enum ACTOR_ID;

struct CollideSelect{
	ACTOR_ID otherID;
	COL_ID colID;
};

class WorldActor{
public:
	WorldActor();
	~WorldActor();
	void Update(float frameTime);
	void Draw() const;
	void Add(ACTOR_ID id, ActorPtr actor);
	void Clear();
	void SetCollideSelect(ActorPtr thisActor, ACTOR_ID otherID, COL_ID colID);

private:
	typedef std::shared_ptr<ActorManager> ActorManagerPtr;
	typedef std::map<ACTOR_ID,ActorManagerPtr> ActorManagerPtrMap;
	typedef std::pair<ACTOR_ID, ActorManagerPtr> ActorManagerPair;
	ActorManagerPtrMap managers;
	std::map<ActorPtr, std::vector<CollideSelect>> colselect;
};