#pragma once
#include "../Other/DX11User.h"
#include <dinput.h>
#include "../Other/D3D11User.h"

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif
namespace INPUTKEY
{
	const UINT KEY_RIGHT = DIK_RIGHTARROW;
	const UINT KEY_LEFT = DIK_LEFTARROW;
	const UINT KEY_UP = DIK_UPARROW;
	const UINT KEY_DOWN = DIK_DOWNARROW;
	const UINT KEY_A = DIK_A;
	const UINT KEY_W = DIK_W;
	const UINT KEY_S = DIK_S;
	const UINT KEY_D = DIK_D;
	const UINT KEY_R = DIK_R;
	const UINT KEY_F = DIK_F;
	const UINT KEY_Z = DIK_Z;
	const UINT KEY_X = DIK_X;
	const UINT KEY_C = DIK_C;
	const UINT KEY_LSHIFT = DIK_LSHIFT;
	const UINT KEY_LCTRL = DIK_LCONTROL;
	const UINT KEY_SPACE = DIK_SPACE;
	const UINT KEY_1 = DIK_1;
	const UINT KEY_2 = DIK_2;
	const UINT KEY_3 = DIK_3;
	const UINT KEY_4 = DIK_4;
	const UINT KEY_5 = DIK_5;
	const UINT KEY_6 = DIK_6;
}

class KeyBoard{
public:
	KeyBoard();
	~KeyBoard();
	void Initialize(D3D11USER* d3d11User, HINSTANCE hInst, LPDIRECTINPUT8 dInput);
	void WindowActiveCheck();
	BYTE KeyDown(const UINT KeyCode,bool trigger = false)const;
	bool KeyUp(const UINT KeyCode)const;
private:
	LPDIRECTINPUTDEVICE8 dInputDevice;
	BYTE keyBoardNowBuf[256],keyBoardOldBuf[256];
	DIMOUSESTATE2 nowDim, oldDim;
public:
};