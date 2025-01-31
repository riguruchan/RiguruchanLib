#pragma once
#include "../Other/DX11User.h"
#include "../Other/D3D11User.h"
#include "KeyBoard.h"
#include "Mouse.h"

class Input{
public:
	Input();
	~Input();
	void Initialize(D3D11USER* d3d11User, HINSTANCE hInst, LPDIRECTINPUT8 dInput);
	void WindowActiveCheck(RECT rc);
	bool KeyDown(const UINT KeyCode, bool trigger = false)const;
	bool KeyUp(const UINT KeyCode)const;
	bool MouseButtonDown(INPUTMOUSEBUTTON id, bool trriger = false);
	bool MouseButtonDownTrigger(INPUTMOUSEBUTTON id);
	bool MouseButtonUp(INPUTMOUSEBUTTON id);
	Vector3 MouseVec();
private:
	KeyBoard k;
	Mouse m;
public:
};