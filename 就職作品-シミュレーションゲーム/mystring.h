#pragma once
#include	<DirectXMath.h>

namespace MyString {

#define		MAXMOJICNT		1024

// 頂点バッファ更新
static void UpdateVertexBuffer();

// インデックスバッファ更新
static void UpdateIndexBuffer();

// 戻り値　インデックス数
int CreateVertexIndexData(int length);

// 初期化
bool InitMyString();

// 文字列描画
void DrawMyString();

// 描画する文字列をクリア
void ClearMyString();

// アスキーフォント描画用のＵＶ座標取得
void GetAsciiFontUV(char code, DirectX::XMFLOAT2* uv);

//　文字列描画
void DrawString(int x, int y, int width, int height, DirectX::XMFLOAT4 color, const char* str);

}

