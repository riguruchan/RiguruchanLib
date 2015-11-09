matrix g_matW;
matrix g_matV;
matrix g_matP;

// テクスチャー
Texture2D g_Tex;

// サンプラーステート
SamplerState  g_Sampler;

float4 g_vecLight;

float4 materialColor;

float4x4 boneMatrix[256];
float4x4 otherBoneMatrix[256];

float4 blendLevel;
// 頂点シェーダーの入力パラメータ
struct VS_IN
{
	float4 pos   : POSITION;   // 頂点座標
	float3 normal : NORMAL;      // 頂点カラー
	float2 texel : TEXCOORD;   // テクセル
	uint4 boneIndex : BONEINDEX;
	uint4 weight : WEIGHT;
};

// 頂点シェーダーの出力パラメータ
struct VS_OUT
{
	float4 pos   : SV_POSITION;
	float3 normal : NORMAL;
	float2 texel : TEXCOORD0;
};

// 頂点シェーダー
VS_OUT VS_Main(VS_IN In)
{
	VS_OUT Out;
	Out = (VS_OUT)0;


	float4 pos = In.pos;
		float3 nor = In.normal;

		//pos = float4(mul(pos, oneDirectXMat), 1);
		//nor = float4(mul(nor, oneDirectXMat), 1);
		//float4 pos = float4(In.pos.x, In.pos.z, In.pos.y, In.pos.w);
		//float3 nor = float3(In.normal.x, In.normal.z, In.normal.y);
		pos = In.pos;
	nor = In.normal;
	float4x3 bm0 = boneMatrix[In.boneIndex[0]];//*directXMat;
		float4x3 bm1 = boneMatrix[In.boneIndex[1]];//*directXMat;
		float4x3 bm2 = boneMatrix[In.boneIndex[2]];//*directXMat;
		float4x3 bm3 = boneMatrix[In.boneIndex[3]];//*directXMat;

		float3 bpos0 = In.weight[0] * mul(pos, bm0);
		float3 bpos1 = In.weight[1] * mul(pos, bm1);
		float3 bpos2 = In.weight[2] * mul(pos, bm2);
		float3 bpos3 = In.weight[3] * mul(pos, bm3);

		float3 bnor0 = In.weight[0] * mul(nor, bm0);
		float3 bnor1 = In.weight[1] * mul(nor, bm1);
		float3 bnor2 = In.weight[2] * mul(nor, bm2);
		float3 bnor3 = In.weight[3] * mul(nor, bm3);

		float4x3 am0 = otherBoneMatrix[In.boneIndex[0]];//*directXMat;
		float4x3 am1 = otherBoneMatrix[In.boneIndex[1]];//*directXMat;
		float4x3 am2 = otherBoneMatrix[In.boneIndex[2]];//*directXMat;
		float4x3 am3 = otherBoneMatrix[In.boneIndex[3]];//*directXMat;

		float3 apos0 = In.weight[0] * mul(pos, am0);
		float3 apos1 = In.weight[1] * mul(pos, am1);
		float3 apos2 = In.weight[2] * mul(pos, am2);
		float3 apos3 = In.weight[3] * mul(pos, am3);
											   
		float3 anor0 = In.weight[0] * mul(nor, am0);
		float3 anor1 = In.weight[1] * mul(nor, am1);
		float3 anor2 = In.weight[2] * mul(nor, am2);
		float3 anor3 = In.weight[3] * mul(nor, am3);

	bpos0 = lerp(bpos0, apos0, blendLevel.x);
	bpos1 = lerp(bpos1, apos1, blendLevel.x);
	bpos2 = lerp(bpos2, apos2, blendLevel.x);
	bpos3 = lerp(bpos3, apos3, blendLevel.x);

	bnor0 = lerp(bnor0, anor0, blendLevel.x);
	bnor1 = lerp(bnor1, anor1, blendLevel.x);
	bnor2 = lerp(bnor2, anor2, blendLevel.x);
	bnor3 = lerp(bnor3, anor3, blendLevel.x);

		//if (In.weight[0] != 0)
		pos.xyz = (bpos0.xyz + bpos1.xyz + bpos2.xyz + bpos3.xyz) / 100.0;
	nor.xyz = (bnor0.xyz + bnor1.xyz + bnor2.xyz + bnor3.xyz) / 100.0;
	pos = mul(pos, g_matW);
	Out.pos = mul(pos, g_matV);
	Out.pos = mul(Out.pos, g_matP);

	nor = mul(nor, g_matW);
	nor = mul(nor, g_matV);

	Out.normal = normalize(nor);
	Out.texel = float2(In.texel.x, 1 - In.texel.y);

	return Out;
}

// 頂点シェーダー
VS_OUT VS_MainNonAni(VS_IN In)
{
	VS_OUT Out;
	Out = (VS_OUT)0;


	float4 pos = In.pos;
	float3 nor = In.normal;

		pos = mul(pos, g_matW);
	Out.pos = mul(pos, g_matV);
	Out.pos = mul(Out.pos, g_matP);

	Out.normal = normalize(nor);
	Out.texel = float2(In.texel.x, 1 - In.texel.y);

	return Out;
}

// ピクセルシェーダ
float4 PS_Texture(VS_OUT In) : SV_TARGET
{
	// ランバート拡散照明
	float Color = dot(-normalize(g_vecLight.xyz), normalize(In.normal));
	float4 dif = g_Tex.Sample(g_Sampler, In.texel);
	float3 c = dif.rgb  *max(Color,0.3f);
	return float4(c,dif.a);
}
// エントリーポイントとバージョンを指定する
technique11 TShader
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Main()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
		SetComputeShader(NULL);
	}
	pass P1
	{
		SetVertexShader(CompileShader(vs_5_0, VS_MainNonAni()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
		SetComputeShader(NULL);
	}
}