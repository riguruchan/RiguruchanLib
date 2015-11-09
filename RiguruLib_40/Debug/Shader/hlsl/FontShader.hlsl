matrix g_matW;
matrix g_matV;
matrix g_matP;

// �e�N�X�`���[
Texture2D g_Tex;

// �T���v���[�X�e�[�g
SamplerState  g_Sampler;

float4 materialColor;

float4 texSizeFontSizeFontSelect;
// ���_�V�F�[�_�[�̓��̓p�����[�^
struct VS_IN
{
	float4 pos   : POSITION;   // ���_���W
	float3 normal : NORMAL;      // ���_�J���[
	float2 texel : TEXCOORD;   // �e�N�Z��
	uint4 boneIndex : BONEINDEX;
	uint4 weight : WEIGHT;
};

// ���_�V�F�[�_�[�̏o�̓p�����[�^
struct VS_OUT
{
	float4 pos   : SV_POSITION;
	float3 normal : NORMAL;
	float2 texel : TEXCOORD0;
};

// ���_�V�F�[�_�[
VS_OUT VS_Main(VS_IN In)
{
	VS_OUT Out;
	Out = (VS_OUT)0;
	float texSize = texSizeFontSizeFontSelect.x;
	float fontSize = texSizeFontSizeFontSelect.y;
	float2 fontSelect = float2(texSizeFontSizeFontSelect.z, texSizeFontSizeFontSelect.w);
		float4 pos = float4(
		(1.0f/texSize) *fontSize* In.pos.x,
		(1.0f / texSize)*fontSize * In.pos.y,
		In.pos.z,
		In.pos.w);
		float3 nor = In.normal;

	pos = mul(pos, g_matW);
	Out.pos = mul(pos, g_matV);
	Out.pos = mul(Out.pos, g_matP);
	Out.texel = float2(
		(1.0f / texSize) *fontSize * In.texel.x + fontSize / texSize * fontSelect.x,
		(1.0f / texSize) *fontSize * (1.0f - In.texel.y) + fontSize / texSize* fontSelect.y
		);
	return Out;
}

// �s�N�Z���V�F�[�_
float4 PS_Main(VS_OUT In) : SV_TARGET
{
	return  g_Tex.Sample(g_Sampler, In.texel);// *materialColor;
}

// �G���g���[�|�C���g�ƃo�[�W�������w�肷��
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