#pragma once
#include "../../Other/DX11User.h"
#include "RgrALoader.h"
#include "../Shader/Shader.h"

class Animation{
public:
	Animation();
	~Animation();
	//�A�j���[�V�����̃��[�h
	void Load(const char* animName);
	//�A�j���[�V�����̃o�C���h
	void BindAnim(Shader* shader, int animCount);
	//�A�j���[�V�����̃o�C���h(�u�����h)
	void BindAnimBlend(Animation& ani, Shader* shader, int animCount, int otherAnimCount, float blendLevel);
	AnimUser* ReturnAnimUser();
private:
	RgrALoader rl;
	AnimUser au;
};