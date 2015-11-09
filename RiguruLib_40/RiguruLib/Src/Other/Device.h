#pragma once
#include "DX11User.h"
#include "D3D11User.h"
#include "../Actor/Camera/Camera.h"
#include "../Input/Input.h"

class Device{
public:
	Device(){
	}
	~Device(){
	}
	static Device &GetInstance(){
		static Device d;
		return d;
	}

	void Initialize(){
		input = new Input();
		d3d11User = new D3D11USER();
	}

	void CameraInit(std::shared_ptr<Stage> stage){
		camera = new Camera(stage);
	}

	void ClearAll(){
		SAFE_DELETE(camera);
		SAFE_DELETE(input);

		d3d11User->Invalidate();
		SAFE_DELETE(d3d11User);
	}

	Camera* GetCamera(){
		return camera;
	}

	Input* GetInput(){
		return input;
	}

	D3D11USER* Getd3d11User(){
		return d3d11User;
	}
private:
	Camera* camera;
	Input* input;
	D3D11USER* d3d11User;
};