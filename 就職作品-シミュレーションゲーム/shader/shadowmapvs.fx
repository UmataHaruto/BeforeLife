//--------------------------------------------------------------------------------------
// shadowmap�𐶐�����
//--------------------------------------------------------------------------------------

cbuffer ConstantBufferWorld : register(b0)
{
	matrix World;
}

cbuffer ConstantBufferShadowmap : register(b6)
{
	matrix ViewFromLight;				// �����̈ʒu�J����
	matrix ProjectionFromLight;			// �����̈ʒu�J�����ɑΉ������v���W�F�N�V�����ϊ��s��
	matrix ScreenToUVCoord;				// �X�N���[�����W���e�N�X�`�����W��Ԃɕϊ�
}

//--------------------------------------------------------------------------------------
// �\���̒�`
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	float4 Depth	: TEXCOORD0;			// �[�x�l
};

//--------------------------------------------------------------------------------------
// ���_�V�F�[�_�[
//--------------------------------------------------------------------------------------
VS_OUTPUT main(float4 Pos	:	POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, World);
	output.Pos = mul(output.Pos, ViewFromLight);
	output.Pos = mul(output.Pos, ProjectionFromLight);

	output.Depth = output.Pos;

	return output;
}
