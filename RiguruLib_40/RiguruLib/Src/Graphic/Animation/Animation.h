#pragma once
#include "../../Other/DX11User.h"
#include "RgrALoader.h"
#include "../Shader/Shader.h"

class Animation{
public:
	Animation();
	~Animation();
	//アニメーションのロード
	void Load(const char* animName);
	//アニメーションのバインド
	void BindAnim(Shader* shader, int animCount);
	//アニメーションのバインド(ブレンド)
	void BindAnimBlend(Animation& ani, Shader* shader, int animCount, int otherAnimCount, float blendLevel);
	AnimUser* ReturnAnimUser();
private:
	RgrALoader rl;
	AnimUser au;
};