#pragma once
#include "Texture/Texture.h"
#include "Rgr/Mesh.h"
#include "../Actor/ID.h"
#include "../Math/Matrix4.h"
#include "Shader\Shader.h"
#include "Animation\Animation.h"
#include "Shape\Line.h"
#include "Texture\Font.h"
#include "../Math/Vector2.h"
#include "Rgr\OctreeUser.h"
#include "Rgr\OctreeLoader.h"

struct FontParameter{
	FONT_ID id;
	Vector2 pos;
	Vector2 size;
	float range;
	std::string s;
	Vector3 color;
	float alpha;
};
class Graphic{
public:
	Graphic();
	~Graphic();
	static Graphic &GetInstance(){
		static Graphic d;
		return d;
	}
	void Release();
	void LoadTexture(TEXTURE_ID id, const char* modelName);
	void DrawTexture(TEXTURE_ID id, const Matrix4* mat);
	void DrawTexture(TEXTURE_ID id, const Vector2 pos, Vector2 size);
	void LoadMesh(MODEL_ID id, const char* modelName);
	void DrawMesh(MODEL_ID id, const Matrix4* mat,D3DXCOLOR* color = NULL, bool alphaFlag = false);
	MeshUser* ReturnMeshUser(MODEL_ID id);

	void LoadFont(FONT_ID id, const char* modelName);
	//std::to_StringÇ≈êîílÇï∂éöóÒÇ…ïœä∑Ç≈Ç´Ç‹Ç∑
	void DrawFont(FONT_ID id, const Vector2 pos, Vector2 size, float range, std::string s, const Vector3 color = vector3(0,0,0), const float alpha = 1);
	//std::to_StringÇ≈êîílÇï∂éöóÒÇ…ïœä∑Ç≈Ç´Ç‹Ç∑
	void DrawFont(FONT_ID id, const Matrix4* mat, std::string s, const Vector3 color = vector3(0,0,0), const float alpha = 1);

	void DrawAllFont();


	void LoadAnimation(ANIM_ID id, const char* animName);
	void BindAnimation(ANIM_ID id, int animCount, ANIM_ID otherID = ANIM_ID::NULL_ANIM, int otherAnimCount = 0, float blendLevel = 0.0f);
	
	void LoadShader(SHADER_ID id, const char* shaderName, bool depthEnable = TRUE);
	void SetShader(SHADER_ID id);
	void SetTechniquePass(SHADER_ID id, LPCSTR techniqueName, LPCSTR passName);

	void DrawLine(const Vector3& startPos, const Vector3& endPos, const Vector3 color = vector3(1, 1, 1), const float alpha = 1);
	void DrawSphere(const Vector3 pos, const float radius, const Vector3 color = vector3(1, 1, 1), const float alpha = 1);
	void DrawCube(const Vector3 startPos, const Vector3 endPos,  const Vector3 color = vector3(1, 1, 1), const float alpha = 1);
	void DrawCube(const Matrix4 mat,const Vector3 scale, const Vector3 color = vector3(1, 1, 1), const float alpha = 1);

	void LoadOctree(OCT_ID id,const char* octName);
	OctreeUser* ReturnOctree(OCT_ID id);
private:
	std::map<TEXTURE_ID,Texture> texture;
	std::map<MODEL_ID,Mesh> mesh;
	std::map<FONT_ID,Font> font;
	std::map<ANIM_ID, Animation> animation;
	std::map<SHADER_ID, Shader> shader;
	std::map<OCT_ID, OctreeUser> octree;
	SHADER_ID sID;
	bool animBind;
	Line line;
	std::vector<FontParameter> fontParamVec;
};