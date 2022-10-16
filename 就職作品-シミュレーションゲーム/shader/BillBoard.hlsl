//--------------------------------------------------------------------------------------
// ps.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.hlsl"

//--------------------------------------------------------------------------------------
//  �s�N�Z���V�F�[�_�[
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{
	float4 N = input.Normal;
	float4 L = LightDirection;
	N.w = 0.0f;					// �@���̓x�N�g���Ȃ̂łv�̒l���O�ɂ���B
	L.w = 0.0f;					// ���̕����̓x�N�g���Ȃ̂łv�̒l���O�ɂ���B

	N = normalize(N);
	L = normalize(L);

	float4 texcol = g_Tex.Sample(g_SamplerLinear, input.Tex);
    float4 col = texcol;
	col.a = texcol.a;
	return col;
}

