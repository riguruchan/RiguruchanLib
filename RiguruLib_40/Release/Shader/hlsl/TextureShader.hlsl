float4 g_Pos;
float4 g_Size;

// �e�N�X�`���[
Texture2D g_Tex;

// �T���v���[�X�e�[�g
SamplerState  g_Sampler;
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
	float4 pos = In.pos;
	pos.x = lerp(g_Pos.x, g_Pos.x + g_Size.x, pos.x);
	pos.y = lerp(g_Pos.y, g_Pos.y + g_Size.y, pos.y);
	Out.pos = pos;
	float3 nor = In.normal;
	Out.texel = float2(In.texel.x, 1 - In.texel.y);

	return Out;
}

// �s�N�Z���V�F�[�_
float4 PS_Main(VS_OUT In) : SV_TARGET
{
	return g_Tex.Sample(g_Sampler, In.texel);// *max(Color, 0.4f) * materialColor;
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