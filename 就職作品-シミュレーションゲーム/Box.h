#pragma once
#include	<d3d11.h>
#include	<DirectXMath.h>
#include	<math.h>
#include	<vector>

class Box {
private:
	ID3D11Buffer*		m_pVertexBuffer;	// 頂点バッファ
	ID3D11Buffer*		m_pIndexBuffer;		// インデックスバッファ
	ID3D11VertexShader* m_pVertexShader;	// 頂点シェーダー
	ID3D11PixelShader*  m_pPixelShader[2];		// ピクセルシェーダー
	ID3D11InputLayout*  m_pVertexLayout;	// 頂点レイアウト
	float				m_lengthx;			// 横幅(X)
	float				m_lengthy;			// 高さ(Y)
	float				m_lengthz;			// 奥行き(Z)
	struct Vertex {
		DirectX::XMFLOAT3	Pos;
	};

	struct Face{
		unsigned int idx[3];
	};

	Vertex					m_vertex[8];		// 頂点座標

	Face					m_face[12];			// 面インデックスデータ
	unsigned int			m_facenum = 12;		// 面数

	// インデックスデータを作成する
	void CreateIndex();
	// 頂点データを作成する
	void CreateVertex();

public:

	void Draw();
	bool m_ishit;                            //ヒット時

	// 初期化
	bool Init(
		float width,						// 幅
		float height,						// 高さ
		float depth);						// 奥行き

	// 終了処理
	void Exit();
};