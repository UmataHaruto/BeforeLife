#include	<DirectXMath.h>
#include	"DX11util.h"
#include	"dx11mathutil.h"
#include    "StageHitCollider.h"
#include	"CCamera.h"
#include	"CDirectInput.h"
#include    "GameButton.h"

using namespace DirectX;

/*----------------------------------------------
	マウス座標をワールド座標に変換する
------------------------------------------------*/
void MousePosToWorld(const XMFLOAT4X4& mtxView,		// ビュー変換行列
	const XMFLOAT4X4& mtxProjection,				// プロジェクション変換行列
	float MouseX,									// マウス座標　Ｘ
	float MouseY,									// マウス座標　Ｙ
	float z,										// Ｚ値（０．０fから１．０f）
	float screen_width,								// スクリーン高さ
	float screen_height,							// スクリーン幅
	XMFLOAT3& worldpos) {							// ニア側ワールド座標

	XMFLOAT4X4		mtxInvTotal;		// 逆行列
	XMFLOAT4X4		InvView, InvProjection, InvViewPort;

	XMFLOAT4X4		mtxViewPort;		// ビューポート行列

	XMFLOAT3		screenpoint(MouseX, MouseY, z);

	// ビューポート行列生成
	DX11MtxIdentity(mtxViewPort);
	mtxViewPort._11 = screen_width / 2.0f;
	mtxViewPort._22 = -screen_height / 2.0f;
	mtxViewPort._41 = screen_width / 2.0f;
	mtxViewPort._42 = screen_height / 2.0f;

	// 逆行列を計算
	DX11MtxInverse(InvView, mtxView);
	DX11MtxInverse(InvProjection, mtxProjection);
	DX11MtxInverse(InvViewPort, mtxViewPort);

	XMFLOAT4X4 tempmtx;
	// スクリーン座標（ニアクリップ）
	DX11MtxMultiply(tempmtx, InvViewPort, InvProjection);
	DX11MtxMultiply(mtxInvTotal, tempmtx, InvView);

	// 行列×ベクトル
//	DX11Vec3MulMatrix(worldpos, screenpoint, mtxInvTotal);

	worldpos.x = screenpoint.x * mtxInvTotal._11 +
		screenpoint.y * mtxInvTotal._21 +
		screenpoint.z * mtxInvTotal._31 +
		1.0f * mtxInvTotal._41;

	worldpos.y = screenpoint.x * mtxInvTotal._12 +
		screenpoint.y * mtxInvTotal._22 +
		screenpoint.z * mtxInvTotal._32 +
		1.0f * mtxInvTotal._42;

	worldpos.z = screenpoint.x * mtxInvTotal._13 +
		screenpoint.y * mtxInvTotal._23 +
		screenpoint.z * mtxInvTotal._33 +
		1.0f * mtxInvTotal._43;

	float w = screenpoint.x * mtxInvTotal._14 +
		screenpoint.y * mtxInvTotal._24 +
		screenpoint.z * mtxInvTotal._34 +
		1.0f * mtxInvTotal._44;

	worldpos.x = worldpos.x / w;
	worldpos.y = worldpos.y / w;
	worldpos.z = worldpos.z / w;

	return;
}

XMFLOAT3 MousePosWPosFar(uint32_t mousex, uint32_t mousey) {

	// プロジェクション変換行列を取得
	XMFLOAT4X4 mtxprojection = CCamera::GetInstance()->GetProjectionMatrix();

	// ビュー変換行列を取得
	XMFLOAT4X4 mtxview = CCamera::GetInstance()->GetCameraMatrix();

	float screen_width = static_cast<float>(DX11GetScreenWidth());
	float screen_height = static_cast<float>(DX11GetScreenHeight());

	XMFLOAT3 worldpos;

	MousePosToWorld(mtxview,		// ビュー変換行列
		mtxprojection,				// プロジェクション変換行列
		mousex,						// マウス座標　Ｘ
		mousey,						// マウス座標　Ｙ
		1.0f,						// Ｚ値（０．０fから１．０f）
		screen_width,				// スクリーン高さ
		screen_height,				// スクリーン幅
		worldpos);

	return worldpos;
}

XMFLOAT3 MousePosWPosNear(uint32_t mousex,uint32_t mousey) {

	// プロジェクション変換行列を取得
	XMFLOAT4X4 mtxprojection = CCamera::GetInstance()->GetProjectionMatrix();

	// ビュー変換行列を取得
	XMFLOAT4X4 mtxview = CCamera::GetInstance()->GetCameraMatrix();

	float screen_width = static_cast<float>(DX11GetScreenWidth());
	float screen_height = static_cast<float>(DX11GetScreenHeight());

	XMFLOAT3 worldpos;

	MousePosToWorld(mtxview,		// ビュー変換行列
		mtxprojection,				// プロジェクション変換行列
		mousex,						// マウス座標　Ｘ
		mousey,						// マウス座標　Ｙ
		0.0f,						// Ｚ値（０．０fから１．０f）
		screen_width,				// スクリーン高さ
		screen_height,				// スクリーン幅
		worldpos);

	return worldpos;
}

XMFLOAT3 SearchMousePoint(XMFLOAT3& nearp,XMFLOAT3& farp) {

	// マウスＸ座標　Ｙ座標を取得
	uint32_t mousex = CDirectInput::GetInstance().GetMousePosX();
	uint32_t mousey = CDirectInput::GetInstance().GetMousePosY();

	farp = MousePosWPosFar(mousex, mousey);
	nearp = MousePosWPosNear(mousex, mousey);

	XMFLOAT3 direction;

	direction.x = farp.x - nearp.x;
	direction.y = farp.y - nearp.y;
	direction.z = farp.z - nearp.z;

	XMFLOAT3 ans;

	XMFLOAT3 camerapos = CCamera::GetInstance()->GetEye();

	// カメラに一番近い場所をとってくる
	bool sts = SearchAllSurfaceNearestCamera(camerapos, direction, ans);
	if (sts) {
		GameButton::GetInstance().SetMouseWorld(ans);
		return ans;
	}
	else {
		return farp;
	}
}
