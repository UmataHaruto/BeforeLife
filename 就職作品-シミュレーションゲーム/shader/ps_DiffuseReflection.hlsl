//--------------------------------------------------------------------------------------
// ps.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.hlsl"

float WhiteNoise(float2 coord) {
	return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}

//--------------------------------------------------------------------------------------
//  ピクセルシェーダー
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{
	float4 N = input.Normal;
	float4 L = LightDirection;
	N.w = 0.0f;					// 法線はベクトルなのでＷの値を０にする。
	L.w = 0.0f;					// 光の方向はベクトルなのでＷの値を０にする。

	N = normalize(N);
	L = normalize(L);

	float d = max(0.0, dot(L, N));
	float4 diffuse = diffuseMaterial * d;

	//ノイズを生成
	float noise = WhiteNoise(input.Tex) - 0.5;

	float4 ocolor = { noise,noise,noise,1.0f };

	float4 H;
	float4 V = normalize(EyePos - input.WPos);
	H = normalize(L + V);

	float s = max(0, dot(N, H));
	s = pow(s, 50);
	float4 specular = s * specularMaterial;

	float4 texcol = g_Tex.Sample(g_SamplerLinear, input.Tex);
	float4 col = Ambient + specular + diffuse * texcol;
	col.r = ocolor.r;
	col.g = ocolor.g;
	col.b = ocolor.b;

	col.a = texcol.a;
	return col;
}