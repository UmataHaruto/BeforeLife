//--------------------------------------------------------------------------------------
// vs.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.fx"

//--------------------------------------------------------------------------------------
// ���_�V�F�[�_�[
//--------------------------------------------------------------------------------------
VS_OUTPUT main(float4 Pos		:	POSITION,
	float4 Normal : NORMAL,
	float2 Tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, World);
	output.WPos = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// ���[�J�����W���s�N�Z���V�F�[�_�[�ɓn��
	output.LPos = Pos;
	output.LPos.w = 1.0f;

	float4 N = Normal;
	N.w = 0.0f;					// �@���̓x�N�g���Ȃ̂łv�̒l���O�ɂ���B

	N = normalize(N);			// ���K��

	output.Normal = N;

	output.Tex = Tex;

	output.Color = 0;
	output.Color2 = 0;

	// ��������̋������v�Z
	float4 lengthfromlight;
	lengthfromlight = mul(Pos, World);
	lengthfromlight = mul(lengthfromlight, ViewFromLight);
	lengthfromlight = mul(lengthfromlight, ProjectionFromLight);
	output.LengthFromLight = lengthfromlight;

	// �V���h�E�}�b�v���Q�Ƃ��邽�߂̂t�u���W�쐬
	output.ShadowTex = mul(lengthfromlight, ScreenToUVCoord);

	return output;
}