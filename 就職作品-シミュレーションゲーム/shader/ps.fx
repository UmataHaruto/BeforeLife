//--------------------------------------------------------------------------------------
// ps.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.fx"

//--------------------------------------------------------------------------------------
//  �s�N�Z���V�F�[�_�[
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{
	float4 col;
	float4 texcol = g_Tex.Sample(g_SamplerLinear, input.Tex);

	float4 N;
	N = input.Normal;
	N.w = 0.0f;
	N = normalize(N);

	float4 L = invLightDirection;
	L = normalize(L);
	L.w = 0.0f;

	float d;
	d = max(0.0f, dot(L, N));	// �����o�[�g�̗]�����ŉA�e��t����

	float4 diffuse;
	diffuse = d * diffuseMaterial;

	// �X�y�L�����̌v�Z
	float s;

	float4 V;					// �����x�N�g��
	V = invEyePos - input.LPos;
	V = normalize(V);

	float4 H;
	H = L + V;					// �n�[�t�x�N�g��
	H = normalize(H);

	s = pow(max(0.0f, dot(N, H)), 3000);

	float4 specular;
	specular = s;

	//col = texcol * diffuse+specular;
	col = texcol;

	col.r *= AmbientColor.r;
	col.g *= AmbientColor.g;
	col.b *= AmbientColor.b;


	col.a = 1.0f;
	return col;
}