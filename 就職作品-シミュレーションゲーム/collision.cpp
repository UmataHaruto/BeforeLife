//*****************************************************************************
//!	@file	collision.cpp
//!	@brief	
//!	@note	当たり判定に使用するための関数群
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<math.h>
#include	<DirectXMath.h>
#include	<windows.h>
#include	<stdio.h>
#include	"collision.h"
#include	"dx11mathutil.h"

//==============================================================================
//!	@fn		CreatePlaneInfo
//!	@brief　平面の方程式を求める（頂点は時計回りで渡す）
//!	@param	頂点１座標
//!	@param	頂点２座標
//!	@param	頂点３座標
//!	@param	平面の方程式
//!	@retval	なし
//==============================================================================
void CreatePlaneInfo(const XMFLOAT3& p1,
	const XMFLOAT3& p2,
	const XMFLOAT3& p3,
	PLANE& plane)
{
	XMFLOAT3		p1p2;
	XMFLOAT3		p2p3;
	XMFLOAT3		normal;

	p1p2.x = p2.x - p1.x;
	p1p2.y = p2.y - p1.y;
	p1p2.z = p2.z - p1.z;

	p2p3.x = p3.x - p2.x;
	p2p3.y = p3.y - p2.y;
	p2p3.z = p3.z - p2.z;

	DX11Vec3Cross(normal, p1p2, p2p3);

	DX11Vec3Normalize(normal, normal);

	plane.a = normal.x;
	plane.b = normal.y;
	plane.c = normal.z;

	plane.d = -(plane.a * p1.x + plane.b * p1.y + plane.c * p1.z);
}

//==============================================================================
//!	@fn		LinetoPlaneCross
//!	@brief　直線と平面の交点を求める
//!	@param	平面の方程式
//!	@param	直線の起点
//!	@param	直線の方向ベクトル
//!	@param	交点座標
//!	@retval	true 交点あり　false 交点なし
//==============================================================================
bool LinetoPlaneCross(
	const PLANE&		plane,		//平面の方程式
	const XMFLOAT3&		p0,			//直線の起点
	const XMFLOAT3&		wv,			//直線の方向ベクトル
	XMFLOAT3&			ans)		//交点座標
{
	float t=0;

	XMFLOAT3 normal;				// 平面の法線ベクトル（正規化済み）
	normal.x = plane.a;
	normal.y = plane.b;
	normal.z = plane.c;

	float dot;			// 分母
	// 平行チェック(内積を計算する)
	DX11Vec3Dot(dot, normal, wv);
	if (fabsf(dot) < FLT_EPSILON){
//		MessageBox(nullptr, "平行です。", "平行です。", MB_OK);
		return false;
	}

	float dot2;			// 分子
	DX11Vec3Dot(dot2, normal, p0);

	t = -(dot2 + plane.d)/ dot;

	// ここにｔを求める式を入れる
	ans.x = p0.x + wv.x*t;
	ans.y = p0.y + wv.y*t;
	ans.z = p0.z + wv.z*t;
	return true;
}

bool LinetoPlaneCross(
	const PLANE&		plane,		//平面の方程式
	const XMFLOAT3&		p0,			//直線の起点
	const XMFLOAT3&		wv,			//直線の方向ベクトル
	float&				t,			// 交点位置情報	
	XMFLOAT3&			ans)		//交点座標
{
//	float t = 0;

	XMFLOAT3 normal;				// 平面の法線ベクトル（正規化済み）
	normal.x = plane.a;
	normal.y = plane.b;
	normal.z = plane.c;

	float dot;			// 分母
	// 平行チェック(内積を計算する)
	DX11Vec3Dot(dot, normal, wv);
	if (fabsf(dot) < FLT_EPSILON) {
		//		MessageBox(nullptr, "平行です。", "平行です。", MB_OK);

		printf("平行です \n");
		return false;
	}

	float dot2;			// 分子
	DX11Vec3Dot(dot2, normal, p0);

	t = -(dot2 + plane.d) / dot;

	// ここにｔを求める式を入れる
	ans.x = p0.x + wv.x*t;
	ans.y = p0.y + wv.y*t;
	ans.z = p0.z + wv.z*t;
	return true;
}

//==============================================================================
//!	@fn		CheckInTriangle
//!	@brief　３角形の内部にあるかどうかを判定
//!	@param	３角形の第１頂点座標
//!	@param	３角形の第２頂点座標
//!	@param	３角形の第３頂点座標
//!	@param	判定したい頂点座標
//!	@retval	true 内部　false 外部
//==============================================================================
bool CheckInTriangle(const XMFLOAT3&	a,
						const XMFLOAT3&	b,
						const XMFLOAT3&	c,
						const XMFLOAT3&	p ){
	XMFLOAT3	ab,bc,ca;			// ３辺のベクトル
	XMFLOAT3	ap,bp,cp;			// 内部の点とのベクトル
	XMFLOAT3	normal;				// ３角形の法線ベクトル
	XMFLOAT3	n1,n2,n3;			// ３辺と内部の点との法線ベクトル

	ab.x = b.x - a.x;
	ab.y = b.y - a.y;
	ab.z = b.z - a.z;

	bc.x = c.x - b.x;
	bc.y = c.y - b.y;
	bc.z = c.z - b.z;

	ca.x = a.x - c.x;
	ca.y = a.y - c.y;
	ca.z = a.z - c.z;

	ap.x = p.x - a.x;
	ap.y = p.y - a.y;
	ap.z = p.z - a.z;

	bp.x = p.x - b.x;
	bp.y = p.y - b.y;
	bp.z = p.z - b.z;

	cp.x = p.x - c.x;
	cp.y = p.y - c.y;
	cp.z = p.z - c.z;

	DX11Vec3Cross(normal, ab, bc);		// 外積を計算
	
	DX11Vec3Cross(n1, ab, ap);			// 外積を計算

	DX11Vec3Cross(n2, bc, bp);			// 外積を計算

	DX11Vec3Cross(n3, ca, cp);			// 外積を計算

	float dot;
	DX11Vec3Dot(dot, n1, normal);
	if(dot < 0) return false;			// 為す角度が鈍角

	DX11Vec3Dot(dot, n2, normal);
	if (dot < 0) return false;			// 為す角度が鈍角

	DX11Vec3Dot(dot, n3, normal);
	if (dot < 0) return false;			// 為す角度が鈍角

	return true;
}

//==============================================================================
//!	@fn		LengthPointToPlane
//!	@brief　平面と点の距離を求める	また同時に交点の座標を求める
//!	@param	点の座標
//!	@param	平面の方程式
//!	@param	交点の座標
//!	@retval	点と平面の距離
//==============================================================================
float LengthPointToPlane(	const XMFLOAT3& p,
							const PLANE& plane,
							XMFLOAT3& crosspoint)
{
	float dot_work = plane.a*p.x+plane.b*p.y+plane.c*p.z + plane.d;
	float normal_len = plane.a*plane.a+plane.b*plane.b+plane.c*plane.c;
	
	// 交点を求める
	float t = -1.0f * dot_work / normal_len;

	crosspoint.x = p.x + plane.a * t;
	crosspoint.y = p.y + plane.b * t;
	crosspoint.z = p.z + plane.c * t;

	float length = fabsf(dot_work); // ÷ sqrtf(normal_len);	// 平面の法線を正規化しているのでsqrtは省きました

	return length;
}
//******************************************************************************
//	End of file.
//******************************************************************************
