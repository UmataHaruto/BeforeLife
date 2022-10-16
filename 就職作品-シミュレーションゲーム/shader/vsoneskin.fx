//--------------------------------------------------------------------------------------
// vs.fx
//--------------------------------------------------------------------------------------
#include	"psvscommon.fx"
//--------------------------------------------------------------------------------------
// 頂点シェーダー
//--------------------------------------------------------------------------------------
VS_OUTPUT main(
	float4 Pos		:	POSITION,
	float4 Normal : NORMAL,
	float2 Tex : TEXCOORD,
	int4   BoneIndex : BONEINDEX,
	float4 BoneWeight : BONEWEIGHT
	)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	// 頂点ブレンドの処理
	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; i++)
	{
		comb += BoneMatrix[BoneIndex[i]] * BoneWeight[i];
	}

	output.Pos = mul(Pos, comb);

	output.Pos = mul(output.Pos, World);
	output.WPos = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	float4 N = Normal;
	N.w = 0.0f;					// 法線はベクトルなのでＷの値を０にする。

	N = mul(N, comb);			// 法線ベクトルにコンビネーション行列を掛けて方向を変える
	N = mul(N, World);			// 法線ベクトルにワールド変換行列を掛けて方向を変える
	N = normalize(N);			// 正規化

	output.Normal = N;

	float4 L = LightDirection;
	L = normalize(L);
	L.w = 0.0f;

	float d;
	d = max(0.0f, dot(L, N));	// ランバートの余弦則で陰影を付ける

	float4 diffuse;
	diffuse = d;

	output.Tex = Tex;

	output.Color = diffuse;
	return output;
}