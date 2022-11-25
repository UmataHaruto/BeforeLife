#pragma once
#include	<DirectXMath.h>

void MousePosToWorld(const DirectX::XMFLOAT4X4& mtxView,		// ビュー変換行列
	const DirectX::XMFLOAT4X4& mtxProjection,				// プロジェクション変換行列
	float MouseX,									// マウス座標　Ｘ
	float MouseY,									// マウス座標　Ｙ
	float z,										// Ｚ値（０．０fから１．０f）
	float screen_width,								// スクリーン高さ
	float screen_height,							// スクリーン幅
	DirectX::XMFLOAT3& worldpos);
XMFLOAT3 SearchMousePoint(XMFLOAT3& nearp, XMFLOAT3& farp);