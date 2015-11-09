#include "Input.h"
#include "../Math/Vector3.h"
Input::Input(){
}



Input::~Input(){

}

void Input::Initialize(D3D11USER* d3d11User, HINSTANCE hInst, LPDIRECTINPUT8 dInput){
	k.Initialize(d3d11User, hInst, dInput);
	m.Initialize(d3d11User, hInst, dInput);
}

void Input::WindowActiveCheck(RECT rc){
	k.WindowActiveCheck();
	m.WindowActiveCheck(rc);
}

bool Input::KeyDown(const UINT KeyCode, bool trigger)const{
	return k.KeyDown(KeyCode, trigger);
}

bool Input::KeyUp(const UINT KeyCode)const{
	return k.KeyUp(KeyCode);
}

bool Input::MouseButtonDown(INPUTMOUSEBUTTON id, bool trriger){
	return m.MouseButtonDown(id, trriger);
}

bool Input::MouseButtonDownTrigger(INPUTMOUSEBUTTON id){
	return m.MouseButtonDown(id, true);
}

bool Input::MouseButtonUp(INPUTMOUSEBUTTON id){
	return m.MouseButtonUp(id);
}

Vector3 Input::MouseVec(){
	return m.MouseVec();
}