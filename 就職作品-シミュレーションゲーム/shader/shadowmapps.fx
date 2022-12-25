//--------------------------------------------------------------------------------------
// shadowmapを生成する
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// コンスタントバッファ
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// 構造体定義
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	float4 Depth	: TEXCOORD0;			// 深度値
};

//--------------------------------------------------------------------------------------
// ピクセルシェーダー
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{
	float depth = input.Depth.z / input.Depth.w;
	return depth;
}
