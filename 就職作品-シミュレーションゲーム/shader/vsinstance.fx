//--------------------------------------------------------------------------------------
// vs.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.fx"
//--------------------------------------------------------------------------------------
// 頂点シェーダー
//--------------------------------------------------------------------------------------
VS_OUTPUT main(float4 Pos		:	POSITION,
	float4 Normal : NORMAL,
	float2 Tex : TEXCOORD,
	float4x4 mat : LocalToWorld)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, mat);

	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.WPos = output.Pos;
	output.Tex = Tex;

	float4 N = Normal;
	N.w = 0.0f;					// 法線はベクトルなのでＷの値を０にする。

	//N = mul(N, World);
	N = normalize(N);

	output.Normal = N;

	output.Tex = Tex;

	output.Color = 0;
	output.Color2 = 0;

	// 光源からの距離を計算
	float4 lengthfromlight;
	lengthfromlight = mul(Pos, World);
	lengthfromlight = mul(lengthfromlight, ViewFromLight);
	lengthfromlight = mul(lengthfromlight, ProjectionFromLight);
	output.LengthFromLight = lengthfromlight;

	// シャドウマップを参照するためのＵＶ座標作成
	output.ShadowTex = mul(lengthfromlight, ScreenToUVCoord);

	return output;
}