#include "TitleScene.h"
#include <sstream>
#include <iomanip>
#include "Scene.h"
#include "../actor/Player.h"


//�R���X�g���N�^
TitleScene::TitleScene()
{
}

//�f�X�g���N�^
TitleScene::~TitleScene()
{
}

//�J�n
void TitleScene::Initialize()
{
	mIsEnd = false;
}

void TitleScene::Update(float frameTime)
{
	wa.Update(frameTime);
}

//�`��
void TitleScene::Draw() const
{
	wa.Draw();
}

//�I�����Ă��邩�H
bool TitleScene::IsEnd() const
{
	return mIsEnd;
}

//���̃V�[����Ԃ�
Scene TitleScene::Next() const
{
	return Scene::GamePlay;
}

void TitleScene::End(){
	wa.Clear();
}