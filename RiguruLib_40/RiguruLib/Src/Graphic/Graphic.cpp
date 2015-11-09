#include "Graphic.h"
#include "../Math/Converter.h"
#include<string>
Graphic::Graphic(){
	animBind = false;
	line.Load();
}
Graphic::~Graphic(){
	
}
void Graphic::Release(){
	for (auto i : shader){
		i.second.Release();
	}
	for (auto i = texture.begin(); i != texture.end(); ++i){
		i->second.Release();
	}
	for (auto i = mesh.begin(); i != mesh.end(); ++i){
		i->second.Release();
	}
	for (auto i : font){
		i.second.Release();
	}
	line.Release();
}
void Graphic::LoadTexture(TEXTURE_ID id, const char* modelName){
	texture[id].Load(modelName);
}
void Graphic::DrawTexture(TEXTURE_ID id,const Matrix4* mat_){
	SetShader(SHADER_ID::TEXTURE_SHADER);
	Matrix4 mat = *mat_;
	//texture[id].Draw(&RConvert(&mat),&shader[sID]);
	animBind = false;
}

void Graphic::DrawTexture(TEXTURE_ID id, const Vector2 pos, Vector2 size){
	SetShader(SHADER_ID::TEXTURE_SHADER);
	Matrix4 mat = RCMatrix4::Identity();
	texture[id].Draw(vector3(pos.x,pos.y,0),vector3(size.x,size.y,0), &shader[sID]);
	animBind = false;
}

void Graphic::LoadMesh(MODEL_ID id, const char* modelName){
	mesh[id].Load(modelName);
}
void Graphic::DrawMesh(MODEL_ID id, const Matrix4* mat_,D3DXCOLOR* color, bool alphaFlag){
	Matrix4 mat = *mat_;
	mesh[id].Draw(&RConvert(&mat),animBind,&shader[sID],color,alphaFlag);
	animBind = false;
}
MeshUser* Graphic::ReturnMeshUser(MODEL_ID id){
	return mesh[id].ReturnMeshUser();
}
void Graphic::LoadFont(FONT_ID id, const char* modelName){
	font[id].Load(modelName);
}
void Graphic::DrawFont(FONT_ID id, const Matrix4* mat, std::string s, const Vector3 color, const float alpha){
	//font[id].Draw(&RConvert(mat), &shader[sID], s, &D3DXCOLOR(color.x, color.y, color.z, alpha));
	animBind = false;
}

void Graphic::DrawFont(FONT_ID id, const Vector2 pos, Vector2 size, float range, std::string s, const Vector3 color, const float alpha){
	
	FontParameter fp = { id, pos, size, range, s, color, alpha };
	fontParamVec.push_back(fp);
	animBind = false;
}

void Graphic::DrawAllFont(){
	SetShader(SHADER_ID::FONT_SHADER);
	for (auto i : fontParamVec){
		font[i.id].Draw(i.pos, i.size, i.range, &shader[sID], i.s, &D3DXCOLOR(i.color.x, i.color.y, i.color.z, i.alpha));
	}

	fontParamVec.clear();
}

void Graphic::LoadAnimation(ANIM_ID id, const char* animName){
	animation[id].Load(animName);
}
void Graphic::BindAnimation(ANIM_ID id, int animCount,ANIM_ID otherID,int otherAnimCount,float blendLevel_){
	float blendLevel = blendLevel_;
	if (blendLevel < 0)blendLevel = 0;
	else if (blendLevel > 1)blendLevel = 1;

	if (otherID != ANIM_ID::NULL_ANIM)
	animation[id].BindAnimBlend(animation[otherID], &shader[sID], animCount,otherAnimCount,blendLevel);
	else
	animation[id].BindAnim(&shader[sID], animCount);
	animBind = true;
}
void Graphic::LoadShader(SHADER_ID id, const char* shaderName,bool depthEnable){
	shader[id].CreateShader(shaderName,depthEnable);
	shader[id].SetTechniquePass("TShader", "P0");
	shader[id].CreateLayout();
}
void Graphic::SetShader(SHADER_ID id){
	shader[id].SetShader();
	sID = id;
}
void Graphic::SetTechniquePass(SHADER_ID id, LPCSTR techniqueName, LPCSTR passName){
	shader[id].SetTechniquePass(techniqueName, passName);
}

void Graphic::DrawLine(const Vector3& startPos, const Vector3& endPos, const Vector3 color, const float alpha){
	Graphic::GetInstance().SetShader(SHADER_ID::LINE_SHADER);
	line.Draw(&shader[sID], startPos, endPos, D3DXCOLOR(color.x, color.y, color.z, alpha));
	animBind = false;
}

void Graphic::DrawSphere(const Vector3 pos, const float radius, const Vector3 color, const float alpha){
	Graphic::GetInstance().SetShader(SHADER_ID::SPHERE_SHADER);
	Graphic::GetInstance().SetTechniquePass(SHADER_ID::CUBE_SHADER, "TShader", "P0");
	float size = radius;
	Matrix4 matrix = RCMatrix4::matrix(
		vector3(size, size, size),
		0.0f,
		0.0f,
		0,
		pos);
	mesh[MODEL_ID::SPHERE_MODEL].Draw(&RConvert(&matrix), false, &shader[sID], &D3DXCOLOR(color.x, color.y, color.z, alpha), true);
	animBind = false;
}

void Graphic::DrawCube(const Vector3 startPos,const Vector3 endPos, const Vector3 color, const float alpha){
	Graphic::GetInstance().SetShader(SHADER_ID::CUBE_SHADER);
	Graphic::GetInstance().SetTechniquePass(SHADER_ID::CUBE_SHADER, "TShader", "P0");
	shader[sID].SetVector4("startPos", &D3DXVECTOR4(startPos.x, startPos.y, startPos.z, 1));
	shader[sID].SetVector4("endPos", &D3DXVECTOR4(endPos.x, endPos.y, endPos.z, 1));
	Matrix4 matrix = RCMatrix4::matrix(
		vector3(1,1,1),
		0.0f,
		0.0f,
		0,
		vector3(0,0,0));
	mesh[MODEL_ID::CUBE_MODEL].Draw(&RConvert(&matrix), false, &shader[sID], &D3DXCOLOR(color.x, color.y, color.z, alpha), true);
	animBind = false;
}

void Graphic::DrawCube(const Matrix4 mat,const Vector3 scale, const Vector3 color, const float alpha){
	Graphic::GetInstance().SetShader(SHADER_ID::CUBE_SHADER);
	Graphic::GetInstance().SetTechniquePass(SHADER_ID::CUBE_SHADER, "TShader", "P1");
	Vector3 pos = RCMatrix4::getPosition(mat);
	Vector3 start = - vector3(scale.x / 2, scale.y / 2, scale.z / 2);
	Matrix4 matrix = 
		RCMatrix4::scale(scale) *
		RCMatrix4::translate(start) *
		RCMatrix4::rotateX(RCMatrix4::pitch(mat)) *
		RCMatrix4::rotateY(RCMatrix4::yaw(mat)) *
		RCMatrix4::rotateZ(0)*
		RCMatrix4::translate(pos);
	mesh[MODEL_ID::CUBE_MODEL].Draw(&RConvert(&matrix), false, &shader[sID], &D3DXCOLOR(color.x, color.y, color.z, alpha), true);
	animBind = false;
}

void Graphic::LoadOctree(OCT_ID id, const char* octName){
	OctreeLoader ol;
	ol.Load(octName, &octree[id]);
}

OctreeUser* Graphic::ReturnOctree(OCT_ID id){
	return &octree[id];
}