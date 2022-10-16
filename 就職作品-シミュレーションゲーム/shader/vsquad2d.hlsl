#include	"psvscommon.hlsl"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT main( float4 Pos : POSITION,
			    float4 Color : COLOR,
				float2 Tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	// 座標変換
	Pos.w = 1.0f;
	Pos = mul(Pos, World);

	// スクリーン座標に変換
	output.Pos = Pos;
	output.Pos.x = (Pos.x / ViewportWidth.x) * 2 - 1.0f;
	output.Pos.y = 1.0f - (Pos.y / ViewportHeight.x) * 2;
	output.Pos.z = Pos.z;
	output.Pos.w = 1.0f;

	// カラー値セット
	output.Color = Color;
	// テクスチャ座標セット
	output.Tex = Tex;
	return output;
}
