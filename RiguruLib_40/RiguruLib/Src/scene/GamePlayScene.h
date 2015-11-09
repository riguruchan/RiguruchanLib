#pragma once

#include "IScene.h"
#include "ISceneMediator.h"
#include "../world/World.h"

class  GamePlayScene :public IScene{
public:
	//コンストラクタ 
	GamePlayScene();
	//デストラクタ
	~GamePlayScene();

	//更新前初期化
	virtual void Initialize() override;
	//更新処理
	virtual void Update(float frameTime)override;
	//描画処理
	virtual void Draw() const override;
	//終了しているか？
	virtual bool IsEnd() const override;
	//次のシーンを返す
	virtual Scene Next() const override;
	//終了時処理
	virtual void End()override;

private:
	bool mIsEnd;
	World wa;
	float bvolume;
	float svolume;
};