#include "GamePlayScene.h"
#include <sstream>
#include <iomanip>
#include "Scene.h"
#include "../actor/Player.h"
#include "../actor/Thread.h"
#include "../actor/UI.h"
#include "../Graphic/Graphic.h"
#include "../Audio/Audio.h"
#include "../Actor/Stage.h"


//コンストラクタ
GamePlayScene::GamePlayScene()
{
	//mIsEnd = false;
	svolume = 60;
	bvolume = 60;
}

//デストラクタ
GamePlayScene::~GamePlayScene()
{
}

//開始
void GamePlayScene::Initialize()
{
	mIsEnd = false;
	/************************************************モデル*****************************************************/
	Graphic::GetInstance().LoadMesh(MODEL_ID::SPHERE_MODEL, "Res/Rgr/shape/ball.rgr");
	Graphic::GetInstance().LoadMesh(MODEL_ID::CUBE_MODEL, "Res/Rgr/shape/cube.rgr");
	Graphic::GetInstance().LoadMesh(MODEL_ID::PLAYER_MODEL, "Res/Rgr/kumo/kumo.rgr");
	Graphic::GetInstance().LoadMesh(MODEL_ID::STAGE_MODEL, "Res/Rgr/stage/map01/stageDraw.rgr");

	/***********************************************オクツリー**************************************************/
	Graphic::GetInstance().LoadOctree(OCT_ID::STAGE_OCT, "Res/Rgr/Octree/stageOct.oct");

	/**********************************************アニメーション***********************************************/
	Graphic::GetInstance().LoadAnimation(ANIM_ID::WAIT_ANIM, "Res/RgrA/kumoWait.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::WARKLEFT_ANIM, "Res/RgrA/kumoleft.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::WARKFRONT_ANIM, "Res/RgrA/kumoFront.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::WARKBACK_ANIM, "Res/RgrA/kumoBack.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::WARKRIGHT_ANIM, "Res/RgrA/kumoRight.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::TURNLEFT_ANIM, "Res/RgrA/kumoTurn.rgra");

	Graphic::GetInstance().LoadAnimation(ANIM_ID::STEPLEFT_ANIM, "Res/RgrA/kumoStepleft.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::STEPFRONT_ANIM, "Res/RgrA/kumoStepFront.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::STEPBACK_ANIM, "Res/RgrA/kumoStepBack.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::STEPRIGHT_ANIM, "Res/RgrA/kumoStepRight.rgra");

	Graphic::GetInstance().LoadAnimation(ANIM_ID::DURINGJUMP_ANIM, "Res/RgrA/kumoDuringJump.rgra");
	Graphic::GetInstance().LoadAnimation(ANIM_ID::JUMP_ANIM, "Res/RgrA/kumoJump.rgra");

	Graphic::GetInstance().LoadAnimation(ANIM_ID::ROUND_ANIM, "Res/RgrA/kumoRound.rgra");

	/***********************************************テクスチャ**************************************************/
	Graphic::GetInstance().LoadTexture(TEXTURE_ID::FLOOR_TEXTURE, "Res/Texture/tes.png");
	Graphic::GetInstance().LoadTexture(TEXTURE_ID::CURSOR_TEXTURE, "Res/Texture/cursor.png");
	Graphic::GetInstance().LoadTexture(TEXTURE_ID::MOUSE_TEXTURE, "Res/Texture/mouse.png");

	/**********************************************フォントテクスチャ*******************************************/
	Graphic::GetInstance().LoadFont(FONT_ID::TEST_FONT, "Res/Texture/font/font.png");

	/************************************************シェーダー*************************************************/
	Graphic::GetInstance().LoadShader(SHADER_ID::PLAYER_SHADER, "Shader/cso/fbxModelShader.cso");
	Graphic::GetInstance().LoadShader(SHADER_ID::STAGE_SHADER, "Shader/cso/StageShader.cso");
	Graphic::GetInstance().LoadShader(SHADER_ID::TEXTURE_SHADER, "Shader/cso/TextureShader.cso", false);
	Graphic::GetInstance().LoadShader(SHADER_ID::FONT_SHADER, "Shader/cso/FontShader.cso", false);
	Graphic::GetInstance().LoadShader(SHADER_ID::SPHERE_SHADER, "Shader/cso/SphereShader.cso");
	Graphic::GetInstance().LoadShader(SHADER_ID::CUBE_SHADER, "Shader/cso/CubeShader.cso");
	Graphic::GetInstance().LoadShader(SHADER_ID::LINE_SHADER, "Shader/cso/LineShader.cso");

	/**************************************************音楽*****************************************************/
	Audio::GetInstance().LoadSE(SE_ID::BOOM_SE, _T("Res/Sound/torpedoFire.wav"), 10);
	Audio::GetInstance().LoadBGM(BGM_ID::TITLE_BGM, _T("Res/Sound/04 Le Cirque de Sept Couleurs.wav"));

	std::shared_ptr<Stage> stage = std::make_shared<Stage>(wa);
	wa.Add(ACTOR_ID::PLAYER_ACTOR, std::make_shared<Player>(wa,stage));
	wa.Add(ACTOR_ID::UI_ACTOR, std::make_shared<UI>(wa));
	wa.Add(ACTOR_ID::STAGE_ACTOR, stage);

	Device::GetInstance().CameraInit(stage);

	Audio::GetInstance().SetSEVolume(svolume);
	Audio::GetInstance().SetBGMVolume(bvolume);
}

void GamePlayScene::Update(float frameTime)
{
	//カメラの設定
	wa.Update(frameTime);
	Device::GetInstance().GetCamera()->SetCamera(vector3(0, 0.0f, -3.0f), vector3(0, 0, 0),frameTime);

	if (Device::GetInstance().GetInput()->MouseButtonDownTrigger(INPUTMOUSEBUTTON::LEFT_BUTTON)){
		Audio::GetInstance().PlaySE(SE_ID::BOOM_SE);
	}
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_1, true)){
		svolume = svolume - 10 < 0 ? 0 : svolume - 10;
		Audio::GetInstance().SetSEVolume(svolume);
	}
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_2, true)){
		svolume = svolume + 10 > 100 ? 100 : svolume + 10;
		Audio::GetInstance().SetSEVolume(svolume);
	}
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_3, true)){
		bvolume = bvolume - 10 < 0 ? 0 : bvolume - 10;
		Audio::GetInstance().SetBGMVolume(bvolume);
	}
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_4, true)){
		bvolume = bvolume + 10 > 100 ? 100 : bvolume + 10;
		Audio::GetInstance().SetBGMVolume(bvolume);
	}
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_5, true)){
		Audio::GetInstance().PlayBGM(BGM_ID::TITLE_BGM,true);
	}
	if (Device::GetInstance().GetInput()->KeyDown(INPUTKEY::KEY_6, true)){
		Audio::GetInstance().StopBGM(BGM_ID::TITLE_BGM);
	}
}

//描画
void GamePlayScene::Draw() const
{
	wa.Draw();
}

//終了しているか？
bool GamePlayScene::IsEnd() const
{
	return mIsEnd;
}

//次のシーンを返す
Scene GamePlayScene::Next() const
{
	return Scene::Title;
}

void GamePlayScene::End(){
	wa.Clear();
}