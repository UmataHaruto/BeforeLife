#pragma once
#include	<DirectXMath.h>

struct ConstantBufferWorld		// ワールド変換行列
{
	DirectX::XMFLOAT4X4 World;
};

struct ConstantBufferViewPort
{
	unsigned int	ViewportWidth[4];			// ビューポート幅
	unsigned int	ViewportHeight[4];				// ビューポート高さ
};

bool Init2D();
void Exit2D();