//--------------------------------------------------------------------------------------
// shadowmapを生成する
//--------------------------------------------------------------------------------------

cbuffer ConstantBufferWorld : register(b0)
{
	matrix World;
}

cbuffer ConstantBufferShadowmap : register(b6)
{
	matrix ViewFromLight;				// 光源の位置カメラ
	matrix ProjectionFromLight;			// 光源の位置カメラに対応したプロジェクション変換行列
	matrix ScreenToUVCoord;				// スクリーン座標をテクスチャ座標空間に変換
}

//--------------------------------------------------------------------------------------
// 構造体定義
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	float4 Depth	: TEXCOORD0;			// 深度値
};

//--------------------------------------------------------------------------------------
// 頂点シェーダー
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
