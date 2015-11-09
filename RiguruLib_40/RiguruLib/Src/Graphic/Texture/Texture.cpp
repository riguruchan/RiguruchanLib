#include "Texture.h"
#include "../../Other/Device.h"
#include <iostream>
#include <fstream>
#include "../Rgr/RgrLoader.h"
#include "../../Math/MathUtility.h"
#include "../../Math/Converter.h"
using namespace std;


Texture::Texture(){
}

Texture::~Texture(){
}

void Texture::Release(){
	SAFE_RELEASE(g_pIndexBufferList[0]);
	SAFE_RELEASE(g_pVertexBufferList[0]);
	SAFE_RELEASE(tex[0]);
}

HRESULT Texture::Load(const char* modelName){
	HRESULT hr = E_FAIL;
	std::vector<char> fileNameVec;
	char slash = '\\';

	ID3D11ShaderResourceView* texture = lt.LoadBindTexture(modelName);
	tex.push_back(texture);
	ID3D11Resource* res;
	texture->GetResource(&res);
	ID3D11Texture2D* tex = static_cast<ID3D11Texture2D*>(res);

	D3D11_TEXTURE2D_DESC dec;
	tex->GetDesc(&dec);
	w = dec.Width ;
	h = dec.Height;
	res->Release();

	std::vector<VERTEX> vertexVec;
	UINT* index = new UINT[6];

	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 3;
	index[4] = 4;
	index[5] = 5;

	vertexVec.push_back({
		(D3DXVECTOR3(1.0f, 1.0f, 0)),
		(D3DXVECTOR3(0, 0, 0)),
		(D3DXVECTOR2(1, 1)),
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	});
	vertexVec.push_back({
		(D3DXVECTOR3(0, 1.0f, 0)),
		(D3DXVECTOR3(0, 0, 0)),
		(D3DXVECTOR2(0, 1)),
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	});
	vertexVec.push_back({
		(D3DXVECTOR3(1.0f, 0, 0)),
		(D3DXVECTOR3(0, 0, 0)),
		(D3DXVECTOR2(1, 0)),
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	});
	vertexVec.push_back({
		(D3DXVECTOR3(0, 1.0f, 0)),
		(D3DXVECTOR3(0, 0, 0)),
		(D3DXVECTOR2(0, 1)),
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	});
	vertexVec.push_back({
		(D3DXVECTOR3(0, 0, 0)),
		(D3DXVECTOR3(0, 0, 0)),
		(D3DXVECTOR2(0, 0)),
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	});
	vertexVec.push_back({
		(D3DXVECTOR3(1.0f, 0, 0)),
		(D3DXVECTOR3(0, 0, 0)),
		(D3DXVECTOR2(1, 0)),
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	});

	int vertexSize = vertexVec.size();
	// 頂点の実データを設定
	VERTEX* v = new VERTEX[vertexSize];
	for (int i = 0; i < vertexSize; i++)
		v[i] = vertexVec[i];

	// 頂点バッファ
	ID3D11Buffer* VertexBuffer = NULL;
	// インデックスバッファ
	ID3D11Buffer* IndexBuffer = NULL;

	// インデックスバッファを作成する。
	hr = Device::GetInstance().Getd3d11User()->CreateIndexBuffer(&IndexBuffer, index, sizeof(UINT)* 6, 0);
	g_pIndexBufferList.push_back(IndexBuffer);
	if (FAILED(hr)) goto EXIT;
	delete(index);

	// 頂点バッファを作成する
	hr = Device::GetInstance().Getd3d11User()->CreateVertexBuffer(&VertexBuffer, v, sizeof(VERTEX)* vertexSize, 0);
	g_pVertexBufferList.push_back(VertexBuffer);
	if (FAILED(hr)) goto EXIT;
	delete(v);

	return S_OK;
EXIT:
	return hr;
}

void Texture::Draw(Vector3 pos,Vector3 size, Shader* shader){
	// 行列をエフェクトに設定

	// 頂点バッファ設定
	//Device::GetInstance().Getd3d11User()->SetBlendState(&Device::GetInstance().Getd3d11User()->GetAlignmentBlendDesc(), 1, FALSE);
	Device::GetInstance().Getd3d11User()->SetAlphaBlend(TRUE);
	Device::GetInstance().Getd3d11User()->SetTopology(D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	float assa[4] = { 0.3f, 0.75f, 0.3f, 0 };
	float wi = 1024;
	float he = 768;// /2;
	Vector3 truePos = vector3(pos.x * 2 / wi - 1.0f - ((1.0f / wi) * w * size.x), pos.y * 2 / he - 1.0f -((1.0f / he) * h * size.y), 0);
	Vector3 trueSize = vector3(size.x * 2.0f * (1.0f / wi * w), size.y * 2.0f * (1.0f / he * h), 0);

	shader->SetVector4("g_Pos", & D3DXVECTOR4(truePos.x,truePos.y,0,1));
	shader->SetVector4("g_Size", &D3DXVECTOR4(trueSize.x, trueSize.y, 0, 1));
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Device::GetInstance().Getd3d11User()->SetVertexBuffer(&g_pVertexBufferList[0], &stride, &offset);

	Device::GetInstance().Getd3d11User()->SetIndexBuffer(g_pIndexBufferList[0]);
	// インデックスバッファ設定
	shader->SetTexture("g_Tex", tex[0]);
	// プリミティブ タイプおよびデータの順序に関する情報を設定
	//Zの時はSTRIP
	shader->ShaderApply();
	Device::GetInstance().Getd3d11User()->DrawIndexed(6, 0, 0);
	Device::GetInstance().Getd3d11User()->SetAlphaBlend(FALSE);
}