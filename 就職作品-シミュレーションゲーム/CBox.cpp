#include	"CBox.h"
#include	"CDirectxGraphics.h"

const float PI = XM_PI;

// インデックスデータを作成
void CBox::CreateIndex() {
	m_face[0].idx[0] = 0;		// 手前  ok
	m_face[0].idx[1] = 1;
	m_face[0].idx[2] = 2;

	m_face[1].idx[0] = 2;
	m_face[1].idx[1] = 3;
	m_face[1].idx[2] = 0;

	m_face[2].idx[0] = 5;		// 奥  ok
	m_face[2].idx[1] = 4;
	m_face[2].idx[2] = 7;

	m_face[3].idx[0] = 7;
	m_face[3].idx[1] = 6;
	m_face[3].idx[2] = 5;

	m_face[4].idx[0] = 4;		// 左側 ok
	m_face[4].idx[1] = 0;
	m_face[4].idx[2] = 7;		

	m_face[5].idx[0] = 0;
	m_face[5].idx[1] = 3;
	m_face[5].idx[2] = 7;

	m_face[6].idx[0] = 1;		// 右側 ok
	m_face[6].idx[1] = 5;
	m_face[6].idx[2] = 6;

	m_face[7].idx[0] = 1;
	m_face[7].idx[1] = 6;
	m_face[7].idx[2] = 2;

	m_face[8].idx[0] = 0;		// 上側 ok
	m_face[8].idx[1] = 4;
	m_face[8].idx[2] = 1;

	m_face[9].idx[0] = 4;
	m_face[9].idx[1] = 5;
	m_face[9].idx[2] = 1;

	m_face[10].idx[0] = 3;		// 下側
	m_face[10].idx[1] = 2;
	m_face[10].idx[2] = 7;

	m_face[11].idx[0] = 6;
	m_face[11].idx[1] = 7;
	m_face[11].idx[2] = 2;
}

// 頂点データを作成
void CBox::CreateVertex() {

	m_vertex[0].Pos.x = -(m_lengthx / 2.0f);
	m_vertex[0].Pos.y =  (m_lengthy / 2.0f);
	m_vertex[0].Pos.z = -(m_lengthz / 2.0f);

	m_vertex[1].Pos.x =  (m_lengthx / 2.0f);
	m_vertex[1].Pos.y =  (m_lengthy / 2.0f);
	m_vertex[1].Pos.z = -(m_lengthz / 2.0f);

	m_vertex[2].Pos.x =  (m_lengthx / 2.0f);
	m_vertex[2].Pos.y = -(m_lengthy / 2.0f);
	m_vertex[2].Pos.z = -(m_lengthz / 2.0f);

	m_vertex[3].Pos.x = -(m_lengthx / 2.0f);
	m_vertex[3].Pos.y = -(m_lengthy / 2.0f);
	m_vertex[3].Pos.z = -(m_lengthz / 2.0f);

	m_vertex[4].Pos.x = -(m_lengthx / 2.0f);
	m_vertex[4].Pos.y =  (m_lengthy / 2.0f);
	m_vertex[4].Pos.z =  (m_lengthz / 2.0f);

	m_vertex[5].Pos.x = (m_lengthx / 2.0f);
	m_vertex[5].Pos.y = (m_lengthy / 2.0f);
	m_vertex[5].Pos.z = (m_lengthz / 2.0f);

	m_vertex[6].Pos.x =  (m_lengthx / 2.0f);
	m_vertex[6].Pos.y = -(m_lengthy / 2.0f);
	m_vertex[6].Pos.z =  (m_lengthz / 2.0f);

	m_vertex[7].Pos.x = -(m_lengthx / 2.0f);
	m_vertex[7].Pos.y = -(m_lengthy / 2.0f);
	m_vertex[7].Pos.z =  (m_lengthz / 2.0f);
	
	for (int i = 0; i < 8; i++) {
		m_vertex[i].Color = m_color;
	}
}

// 描画
void CBox::Draw() {

	ID3D11DeviceContext* devcontext =  CDirectXGraphics::GetInstance()->GetImmediateContext();
	// 頂点バッファをセットする
	unsigned int stride = sizeof(Vertex);
	unsigned  offset = 0;
	devcontext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	devcontext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);			// インデックスバッファをセット
	devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// トポロジーをセット（旧プリミティブタイプ）
	devcontext->IASetInputLayout(m_pVertexLayout);					// 頂点レイアウトセット

	devcontext->VSSetShader(m_pVertexShader, nullptr, 0);			// 頂点シェーダーをセット
	devcontext->PSSetShader(m_pPixelShader, nullptr, 0);			// ピクセルシェーダーをセット

	devcontext->DrawIndexed(m_facenum * 3,		// 描画するインデックス数（面数×３）
		0,										// 最初のインデックスバッファの位置
		0);										// 頂点バッファの最初から使う
}

bool CBox::Init(
	float width,
	float height,
	float depth) {

	bool sts;

	// ＢＢＯＸサイズをセット
	m_lengthx = width;
	m_lengthy = height;
	m_lengthz = depth;

	// BOXのインデックスデータを作成
	CreateIndex();

	// BOXの頂点データを作成
	CreateVertex();

	ID3D11Device* device = CDirectXGraphics::GetInstance()->GetDXDevice();

	// 頂点バッファ作成
	sts = CreateVertexBufferWrite(
		device,
		sizeof(Vertex),			// １頂点当たりバイト数
		8,						// 頂点数
		m_vertex,				// 頂点データ格納メモリ先頭アドレス
		&m_pVertexBuffer);		// 頂点バッファ
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(vertex buffer) error", "Error", MB_OK);
		return false;
	}

	// インデックスバッファ作成
	sts = CreateIndexBuffer(
			device,
			12*3,					// １２面×３頂点
			m_face,					// インデックスデータ先頭アドレス
			&m_pIndexBuffer);		// インデックスバッファ
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(index buffer) error", "Error", MB_OK);
		return false;
	}

	// 頂点データの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	unsigned int numElements = ARRAYSIZE(layout);

	// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
	sts = CreateVertexShader(device,
		"shader/3dboxvs.fx",
		"main",
		"vs_4_0",
		layout,
		numElements,
		&m_pVertexShader,
		&m_pVertexLayout);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
		return false;
	}

	// ピクセルシェーダーを生成
	sts = CreatePixelShader(			// ピクセルシェーダーオブジェクトを生成
		device,							// デバイスオブジェクト
		"shader/3dboxps.fx",
		"main",
		"ps_4_0",
		&m_pPixelShader);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	return true;
}

void CBox::Exit() {

	// ピクセルシェーダー解放
	if (m_pPixelShader) {
		m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}

	// 頂点シェーダー解放
	if (m_pVertexShader) {
		m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}

	if (m_pVertexLayout) {
		m_pVertexLayout->Release();
		m_pVertexLayout = nullptr;
	}

	if (m_pIndexBuffer) {
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}

	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
}