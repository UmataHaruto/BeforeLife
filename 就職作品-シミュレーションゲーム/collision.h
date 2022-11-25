#pragma once
#include	<DirectXMath.h>
#include	"dx11mathutil.h"

// プロトタイプ宣言
// 平面の方程式を求める
void CreatePlaneInfo(	const DirectX::XMFLOAT3& p1,		// 頂点１
						const DirectX::XMFLOAT3& p2,		// 頂点２
						const DirectX::XMFLOAT3& p3 ,		// 頂点３
						PLANE& plane);						// 平面の方程式

// 平面と直線の交点を求める
bool LinetoPlaneCross(	const PLANE& plane,					//平面の方程式
						const DirectX::XMFLOAT3& line_o,	//直線が通る点
						const DirectX::XMFLOAT3& line_v,	//直線の方向ベクトル
							  DirectX::XMFLOAT3& ans);		//交点座標

// 平面と直線の交点を求める
bool LinetoPlaneCross(
	const PLANE&		plane,		//平面の方程式
	const XMFLOAT3&		p0,			//直線の起点
	const XMFLOAT3&		wv,			//直線の方向ベクトル
	float&				t,			// 交点位置情報	
	XMFLOAT3&			ans);		//交点座標


// 内外判定
bool CheckInTriangle(	const DirectX::XMFLOAT3& a,			// 頂点１
						const DirectX::XMFLOAT3& b,			// 頂点２
						const DirectX::XMFLOAT3& c,			// 頂点３
						const DirectX::XMFLOAT3& p);		// 判定する点座標

// 交点を求める
float LengthPointToPlane(	const DirectX::XMFLOAT3& p,		// 点座標
							const PLANE& plane,				// 平面の方程式
							DirectX::XMFLOAT3& crosspoint);	// 交点
