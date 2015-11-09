#pragma once
#include "IWorld.h"
#include "WorldActor.h"

class World :public IWorld{
public:
	World();
	~World();

	void Initialize();
	void Update(float frameTime);
	void Draw() const;
	void Clear();
	virtual void Add(ACTOR_ID id, ActorPtr actor);
	virtual void Add(ActorPtr actor);
	virtual bool IsEnd() const;
	virtual void SetCollideSelect(ActorPtr thisActor, ACTOR_ID otherID, COL_ID colID);

private:
	WorldActor actors;
};