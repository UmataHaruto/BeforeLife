#include "psvscommon.hlsl"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main( VS_OUTPUT input) : SV_Target
{
	//float4 texcol = g_Tex.Sample(g_SamplerLinear,input.Tex);
	//return texcol*input.Color;

	float4 texcol = g_Tex.Sample(g_SamplerLinear, input.Tex);
	float4 col;
	col = texcol;
	col = texcol * input.Color;
	col.a = input.Color.a*texcol.a;
	return col;
}
