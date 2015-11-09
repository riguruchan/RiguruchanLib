#include "TitleScene.h"
#include <sstream>
#include <iomanip>
#include "Scene.h"
#include "../actor/Player.h"


//コンストラクタ
TitleScene::TitleScene()
{
}

//デストラクタ
TitleScene::~TitleScene()
{
}

//開始
void TitleScene::Initialize()
{
	mIsEnd = false;
}

void TitleScene::Update(float frameTime)
{
	wa.Update(frameTime);
}

//描画
void TitleScene::Draw() const
{
	wa.Draw();
}

//終了しているか？
bool TitleScene::IsEnd() const
{
	return mIsEnd;
}

//次のシーンを返す
Scene TitleScene::Next() const
{
	return Scene::GamePlay;
}

void TitleScene::End(){
	wa.Clear();
}