Texture2D g_Tex : register(t0);				// �e�N�X�`��
SamplerState g_SamplerLinear : register(s0);	// �T���v���[

//--------------------------------------------------------------------------------------
// �R���X�^���g�o�b�t�@
//--------------------------------------------------------------------------------------

cbuffer ConstantBufferWorld : register(b0)
{
	matrix World;
}

cbuffer ConstantBufferView : register(b1)
{
	matrix View;
}

cbuffer ConstantBufferProjection : register(b2)
{
	matrix Projection;
}

cbuffer ConstantBufferMaterial : register(b3)
{
	float4	ambientMaterial;
	float4	diffuseMaterial;
	float4	specularMaterial;
}

cbuffer ConstantBufferLight : register(b4)
{
	float4 LightDirection;			// ���̕���
	float4 EyePos;					// ���_�ʒu
	float4 invLightDirection;		// ���̕����i�t�s����|�������̕����j
	float4 invEyePos;				// ���_�ʒu�i�t�s����|�������_�̈ʒu�j
}

cbuffer ConstantBufferBoneMatrix : register(b5)
{
	matrix BoneMatrix[400];
}

cbuffer ConstantBufferShadowmap : register(b6)
{
	matrix ViewFromLight;				// �����̈ʒu�J����
	matrix ProjectionFromLight;			// �����̈ʒu�J�����ɑΉ������v���W�F�N�V�����ϊ��s��
	matrix ScreenToUVCoord;				// �X�N���[�����W���e�N�X�`�����W��Ԃɕϊ�
}

cbuffer ConstantBufferAmbientColor : register(b7)
{
	float4 AmbientColor;
}

cbuffer ConstantBufferHairColor : register(b8)
{
	float4 HairColor;
}
//--------------------------------------------------------------------------------------
// �\���̒�`
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
	float4 Color2			: COLOR1;
	float2 Tex : TEXCOORD;
	float4 WPos : TEXCOORD1;
	float4 Normal : TEXCOORD2;
	float4 LPos				: TEXCOORD3;
	float4 LengthFromLight	: TEXCOORD4;
	float4 ShadowTex		: TEXCOORD5;		// �V���h�E�}�b�v���Q�Ƃ���t�u���W
};

