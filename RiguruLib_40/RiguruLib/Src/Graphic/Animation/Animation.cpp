#include "Animation.h"
#include "../../Math/Matrix4.h"
#include "../../Math/MatrixUtility.h"
#include "../../Math/Converter.h"
Animation::Animation(){

}
Animation::~Animation(){

}

//アニメーションのロード
void Animation::Load(const char* animName){
	rl.Load(animName, &au);
}

//アニメーションのバインド
void Animation::BindAnim(Shader* shader, int animCount){
	D3DXMATRIX* animMat = new D3DXMATRIX[256];
	for (int i = 0; i < 256; i++)D3DXMatrixIdentity(&animMat[i]);
	int animCount_ = abs(animCount);
	int size = (*au.ReturnMat())[0].size() - 1;
	if (size != 0){
		animCount_ %= size;
		if (animCount < 0){
			animCount_ = size - animCount_;
		}
	}
	animCount_ = 0;
	for (int fdf = 0; fdf < (*au.ReturnMat()).size(); fdf++){
		animMat[fdf] = (*au.ReturnMat())[fdf][animCount_];
	}
	shader->SetMatrix("boneMatrix", animMat, 256);
	shader->SetVector4("blendLevel", &D3DXVECTOR4(0, 0, 0, 0));

	SAFE_DELETE(animMat);
}

//アニメーションのバインド(ブレンド)
void Animation::BindAnimBlend(Animation& ani, Shader* shader, int animCount, int otherAnimCount, float blendLevel){
	D3DXMATRIX* animMat1 = new D3DXMATRIX[256];
	for (int i = 0; i < 256; i++)D3DXMatrixIdentity(&animMat1[i]);
	int animCount_ = abs(animCount);
	int size = (*au.ReturnMat())[0].size() - 1;
	
	if (size != 0){
		animCount_ %= size;
		if (animCount < 0){
			animCount_ = size - animCount_;
		}
	}
	else animCount_ = 0;
	for (int fdf = 0; fdf < (*au.ReturnMat()).size(); fdf++){
		animMat1[fdf] = (*au.ReturnMat())[fdf][animCount_];
	}
	D3DXMATRIX* animMat2 = new D3DXMATRIX[256];
	for (int i = 0; i < 256; i++)D3DXMatrixIdentity(&animMat2[i]);
	AnimUser* otherAu = ani.ReturnAnimUser();
	int animCount2_ = abs(otherAnimCount);
	int size2 = (*otherAu->ReturnMat())[0].size() - 1;	
	if (size2 != 0){
		animCount2_ %= size2;
		if (otherAnimCount < 0){
			animCount2_ = size2 - animCount2_;
		}
	}
	else animCount2_ = 0;
	for (int fdf = 0; fdf < (*otherAu->ReturnMat()).size(); fdf++){
		animMat2[fdf] = (*otherAu->ReturnMat())[fdf][animCount2_];
	}

	shader->SetMatrix("boneMatrix", animMat1, 256);
	shader->SetMatrix("otherBoneMatrix", animMat2, 256);
	shader->SetVector4("blendLevel", &D3DXVECTOR4(blendLevel, 0, 0, 0));

	SAFE_DELETE(animMat1);
	SAFE_DELETE(animMat2);
}

AnimUser* Animation::ReturnAnimUser(){
	return &au;
}