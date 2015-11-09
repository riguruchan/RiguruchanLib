float4 g_Pos;
float4 g_Size;

// テクスチャー
Texture2D g_Tex;

// サンプラーステート
SamplerState  g_Sampler;
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
	pos.x = lerp(g_Pos.x, g_Pos.x + g_Size.x, pos.x);
	pos.y = lerp(g_Pos.y, g_Pos.y + g_Size.y, pos.y);
	Out.pos = pos;
	float3 nor = In.normal;
	Out.texel = float2(In.texel.x, 1 - In.texel.y);

	return Out;
}

// ピクセルシェーダ
float4 PS_Main(VS_OUT In) : SV_TARGET
{
	return g_Tex.Sample(g_Sampler, In.texel);// *max(Color, 0.4f) * materialColor;
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
		SetPixelShader(CompileShader(ps_5_0, PS_Main()));
		SetComputeShader(NULL);
	}
}