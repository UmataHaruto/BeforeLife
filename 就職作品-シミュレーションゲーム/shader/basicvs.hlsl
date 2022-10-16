#include	"psvscommon.hlsl"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT main( float4 Pos : POSITION,
			    float4 Color : COLOR,
				float2 Tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	// スクリーン座標計算用
	float4 ScreenPos;

	// 座標変換
	Pos.w = 1.0f;
	ScreenPos = mul(Pos, World);			// 座標変換

	// スクリーン座標に変換
	output.Pos = ScreenPos;
	output.Pos.x = (ScreenPos.x / ViewportWidth.x) * 2 - 1.0f;
	output.Pos.y = 1.0f - (ScreenPos.y / ViewportHeight.x) * 2;

	output.Pos.z = Pos.z;				
	output.Pos.w = 1.0f;

	// カラー値セット
	output.Color = Color;
	// テクスチャ座標セット
	output.Tex = Tex;
	return output;
}
