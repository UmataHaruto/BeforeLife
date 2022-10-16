#pragma once
#include	<d3d11.h>
#include	<DirectXMath.h>
#include	<math.h>
#include	<vector>
#include	"Shader.h"
#include	"CDirectxGraphics.h"

class CBox {
private:
	ID3D11Buffer*		m_pVertexBuffer;	// 頂点バッファ
	ID3D11Buffer*		m_pIndexBuffer;		// インデックスバッファ
	ID3D11VertexShader* m_pVertexShader;	// 頂点シェーダー
	ID3D11PixelShader*  m_pPixelShader;		// ピクセルシェーダー
	ID3D11InputLayout*  m_pVertexLayout;	// 頂点レイアウト
	float				m_lengthx;			// 横幅(X)
	float				m_lengthy;			// 高さ(Y)
	float				m_lengthz;			// 奥行き(Z)

	struct Vertex {
		DirectX::XMFLOAT3	Pos;			// 位置
		DirectX::XMFLOAT4	Color;			// カラー
	};

	struct Face{
		unsigned int idx[3];
	};

	Vertex					m_vertex[8];		// 頂点座標

	Face					m_face[12];			// 面インデックスデータ
	unsigned int			m_facenum = 12;		// 面数

	DirectX::XMFLOAT4		m_color = { 1.0f,1.0f,1.0f,0.3f };	// カラー値

	// インデックスデータを作成する
	void CreateIndex();
	// 頂点データを作成する
	void CreateVertex();
	// 頂点バッファを書き換える
	void UpdateVertexBuffer() {
		ID3D11DeviceContext* devicecontext;

		devicecontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

		D3D11_MAPPED_SUBRESOURCE pData;

		// ボーン行列格納用定数バッファ更新
		HRESULT hr = devicecontext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
		if (SUCCEEDED(hr)) {
			memcpy_s(pData.pData, pData.RowPitch, (void*)&m_vertex, sizeof(Vertex)*8);
			devicecontext->Unmap(m_pVertexBuffer, 0);
		}
	}
public:
	void SetColor(DirectX::XMFLOAT4 color) {
		m_color = color;
		CreateVertex();
		UpdateVertexBuffer();
	}

	XMFLOAT4 GetColor() {
		return m_color;
	}

	void Draw();

	// 初期化
	bool Init(
		float width,						// 幅
		float height,						// 高さ
		float depth);						// 奥行き

	// 終了処理
	void Exit();
};