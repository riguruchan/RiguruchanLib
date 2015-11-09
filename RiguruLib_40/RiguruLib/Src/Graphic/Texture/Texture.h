#pragma once
#include "../../Other/DX11User.h"
#include  "../../Other/D3D11User.h"
#include "../Shader/Shader.h"
#include "../Rgr/MeshUser.h"
#include  "../Rgr/RgrLoader.h"
#include "../Shader/LoadTexture.h"
class Texture{
public:
	Texture();
	~Texture();
	void Release();
	HRESULT Load(const char* modelName);
	void Draw(Vector3 pos, Vector3 size, Shader* shader);
private:
	// 頂点定義
	struct VERTEX
	{
		// 頂点座標
		D3DXVECTOR3   pos;
		// 法線
		D3DXVECTOR3   normal;
		// テクセル
		D3DXVECTOR2 texel;

		UINT boneIndex[4];
		UINT weight[4];
	};
	// 頂点バッファ
	std::vector<ID3D11Buffer*> g_pVertexBufferList;
	// インデックスバッファ
	std::vector<ID3D11Buffer*> g_pIndexBufferList;

	std::vector<int> indexSize;
	LoadTexture lt;
	std::vector<ID3D11ShaderResourceView*> tex;
	int meshCount;
	float w, h;
};