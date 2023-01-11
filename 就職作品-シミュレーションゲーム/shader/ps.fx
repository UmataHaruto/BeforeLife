//--------------------------------------------------------------------------------------
// ps.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.fx"

//--------------------------------------------------------------------------------------
//  ピクセルシェーダー
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
	d = max(0.0f, dot(L, N));	// ランバートの余弦則で陰影を付ける

	float4 diffuse;
	diffuse = d * diffuseMaterial;

	// スペキュラの計算
	float s;

	float4 V;					// 視線ベクトル
	V = invEyePos - input.LPos;
	V = normalize(V);

	float4 H;
	H = L + V;					// ハーフベクトル
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