#include	<d3d11.h>
#include	<DirectXMath.h>
#include	<wrl/client.h>
#include	<string>
#include	"dx11mathutil.h"
#include	"dx11util.h"
#include	"Shader.h"
#include	"shaderhashmap.h"
#include	"DX11Settransform.h"
#include	"quad2d.h"
#include    "CDirectInput.h"
#include "Sprite2DMgr.h"

// comptr
using Microsoft::WRL::ComPtr;

// 使用する頂点シェーダー名
const char* vsfilename[] = {
	"shader/vsquad2d.hlsl"
};

// 使用するピクセルシェーダー名
const char* psfilename[] = {
	"shader/psquad2d.hlsl"
};

// 矩形の初期化
bool Quad2D::Init(uint32_t width, 
	uint32_t height, 
	DirectX::XMFLOAT4 color, 
	DirectX::XMFLOAT2* uv) {

	DX11MtxIdentity(m_worldmtx);	// 初期姿勢

	// ピクセルシェーダーを生成
	bool sts = ShaderHashmap::GetInstance()->SetPixelShader(psfilename[0]);
	if (!sts) {
		MessageBox(nullptr, "SetPixelShader error", "error", MB_OK);
		return false;
	}

	// 頂点データの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// エレメント数
	unsigned int numElements = ARRAYSIZE(layout);

	// 頂点シェーダーを生成
	sts = ShaderHashmap::GetInstance()->SetVertexShader(
		vsfilename[0],			// 頂点シェーダーファイル名
		layout,					// 頂点レイアウト
		numElements);			// エレメント数

	// 頂点初期化
	updateVertex(width, height, color, uv);

	// デバイスを取得
	ID3D11Device* device;
	device = GetDX11Device();

	// 頂点バッファを生成
	sts = CreateVertexBufferWrite(
		device,						// デバイスオブジェクト
		sizeof(Quad2D::Vertex),		// １頂点当たりバイト数
		4,							// 頂点数
		m_vertex,					// 頂点データ格納メモリ先頭アドレス
		&m_vertexbuffer				// 頂点バッファ
	);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexBuffer error", "error", MB_OK);
		return false;
	}

	// インデックスバッファを生成
	uint32_t index[4] = { 0,1,2,3 };

	sts = CreateIndexBuffer(
		device,						// デバイスオブジェクト
		4,							// インデックス数
		index,						// インデックス格納メモリ先頭アドレス
		&m_indexbuffer				// インデックスバッファ
	);
	if (!sts) {
		MessageBox(nullptr, "Quad2D CreateIndexBuffer error", "error", MB_OK);
		return false;
	}

	// ビューポートサイズセット
	SetViewPortSize(DX11GetScreenWidth(), DX11GetScreenHeight());

	return true;
}

void Quad2D::Update()
{
	//透過
	static bool SwitchInvisible = true;
	XMFLOAT4 color;
	switch (type)
	{
	
	case UILIST::BLACKBACK_START:
		color = m_color;
		color.w -= 0.05;

		SetColor(color);

		if (color.w <= 0)
		{
			SetStatus(UISTATUS::DEAD);
		}
		break;
	
	case UILIST::BLACKBACK_END:
		color = m_color;
		color.w += 0.05;

		SetColor(color);

		if (color.w >= 10)
		{
			SetStatus(UISTATUS::DEAD);
		}
		break;

	case UILIST::CLOUDBACK_START:
		color = m_color;
		color.w -= 0.05;

		SetColor(color);

		if (color.w <= 0)
		{
			SetStatus(UISTATUS::DEAD);
		}
		break;

	case UILIST::CLOUDBACK_END:
		color = m_color;
		color.w += 0.05;

		SetColor(color);

		if (color.w >= 10)
		{
			SetStatus(UISTATUS::DEAD);
		}
		break;

	case UILIST::ICON_MOUSE:
		SetPosition(CDirectInput::GetInstance().GetMousePosX(), CDirectInput::GetInstance().GetMousePosY(),0);
		break;
	default:
		break;
	}
	//アニメーションの場合
	if (m_isAnimation)
	{
		XMFLOAT2 uv[4];

		int index = m_animationNo;

		//横分割数
		int u = index % m_cutU;
		//縦分割数
		int v = index / m_cutU;

		//1コマのサイズ
		float size_x = 1 / (float)m_cutU;
		float size_y = 1 / (float)m_cutV;

		//アニメーション番号に応じてカットを変更

		uv[0].x = u * size_x;
		uv[0].y = v * size_y;
		uv[1].x = (u * size_x) + size_x;
		uv[1].y = v * size_y;
		uv[2].x = u * size_x;
		uv[2].y = (v * size_y) + size_y;
		uv[3].x = (u * size_x) + size_x;
		uv[3].y = (v * size_y) + size_y;

		//uvをセット
		updateVertex(m_width, m_height,m_color, uv);
		updateVbuffer();

		//アニメーション遅延をカウントアップ
		m_delaycount++;

		if (m_delaycount >= m_animationDelay) {
			m_delaycount = 0;
			//アニメーションカウントを進める
			if (m_animationNo < m_cutU * m_cutV - 1) {
				m_animationNo++;
			}
			//ループする場合
			else if (m_isLoop)
			{
				m_animationNo = 0;
			}
			//ターゲットではない場合
			else
			{
				m_sts = UISTATUS::DEAD;
			}
		}
	}

}

// 描画
void Quad2D::Draw() {

	// デバイスコンテキストを取得
	ID3D11DeviceContext* devcontext;
	devcontext = GetDX11DeviceContext();

	// テクスチャをセット
	devcontext->PSSetShaderResources(0, 1, &m_srv);

	// 座標変換用の行列をセット
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, m_worldmtx);

	unsigned int stride = sizeof(Quad2D::Vertex);	// ストライドをセット（１頂点当たりのバイト数）
	unsigned  offset = 0;						// オフセット値をセット

	// 頂点バッファをデバイスコンテキストへセット
	devcontext->IASetVertexBuffers(
		0,									// スタートスロット
		1,									// 頂点バッファ個数
		&m_vertexbuffer,		// 頂点バッファの先頭アドレス
		&stride,							// ストライド
		&offset);							// オフセット

	// インデックスバッファをデバイスコンテキストへセット
	devcontext->IASetIndexBuffer(
		m_indexbuffer,				// インデックスバッファ
		DXGI_FORMAT_R32_UINT,				// フォーマット
		0);									// オフセット

	// トポロジーをセット
	devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 頂点シェーダー、ピクセルシェーダー取得
	ID3D11VertexShader* vs;
	vs = ShaderHashmap::GetInstance()->GetVertexShader(vsfilename[0]);
	ID3D11PixelShader* ps;
	ps = ShaderHashmap::GetInstance()->GetPixelShader(psfilename[0]);

	// 頂点レイアウト取得
	ID3D11InputLayout* layout;
	layout = ShaderHashmap::GetInstance()->GetVertexLayout(vsfilename[0]);

	devcontext->VSSetShader(vs, nullptr, 0);
	devcontext->GSSetShader(nullptr, nullptr, 0);
	devcontext->HSSetShader(nullptr, nullptr, 0);
	devcontext->DSSetShader(nullptr, nullptr, 0);
	devcontext->PSSetShader(ps, nullptr, 0);

	// 頂点フォーマットをセット
	devcontext->IASetInputLayout(layout);

	// ドローコール発行
	devcontext->DrawIndexed(
		4,						// インデックス数
		0,						// 開始インデックス
		0);						// 基準頂点インデックス
}

void Quad2D::UnInit() {
	if (m_vertexbuffer != nullptr) {
		m_vertexbuffer->Release();
		m_vertexbuffer = nullptr;
	}
	if (m_indexbuffer != nullptr) {
		m_indexbuffer->Release();
		m_indexbuffer = nullptr;
	}
	if (m_srv != nullptr) {
		m_srv->Release();
		m_srv = nullptr;
	}
	//if (m_tex != nullptr) {
	//	m_tex->Release();
	//	m_tex = nullptr;
	//}
}

// 拡大、縮小
void Quad2D::SetScale(float sx, float sy, float sz) {
	DX11MtxScale(sx, sy, sz, m_worldmtx);
	m_width = sx;
	m_height = sy;
}

// 位置をセット
void Quad2D::SetPosition(float x, float y, float z) {
	m_worldmtx._41 = x;
	m_worldmtx._42 = y;
	m_worldmtx._43 = z;
}

// Z軸回転
void Quad2D::SetRotation(float angle) {
	DX11MtxRotationZ(angle, m_worldmtx);
}

void Quad2D::SetUV(DirectX::XMFLOAT2* uv)
{
	for (int i = 0; i < 4; i++) {
		m_vertex[i].tex = uv[i];
	}
}

// 頂点データ更新
void Quad2D::updateVertex(uint32_t width, 
	uint32_t height, 
	DirectX::XMFLOAT4 color, 
	DirectX::XMFLOAT2* uv) {

	m_width = static_cast<float>(width);
	m_height = static_cast<float>(height);

	Quad2D::Vertex	v[4] = {
		// 座標													// カラー	// UV	
		XMFLOAT3(	-m_width / 2.0f,	-m_height / 2.0f, 0.0f),	color,		uv[0],
		XMFLOAT3(	 m_width / 2.0f,	-m_height / 2.0f, 0.0f),	color,		uv[1],
		XMFLOAT3(	-m_width / 2.0f,	 m_height / 2.0f, 0.0f),	color,		uv[2],
		XMFLOAT3(	 m_width / 2.0f,	 m_height / 2.0f, 0.0f),	color,		uv[3]
	};

	for (int i = 0; i < 4; i++) {
		m_vertex[i] = v[i];
	}
}

// 頂点バッファ更新
void Quad2D::updateVbuffer() {
	D3D11_MAPPED_SUBRESOURCE pData;
	HRESULT hr = GetDX11DeviceContext()->Map(m_vertexbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		memcpy_s(pData.pData, pData.RowPitch, (void*)(m_vertex), sizeof(Quad2D::Vertex) * 4);
		GetDX11DeviceContext()->Unmap(m_vertexbuffer, 0);
	}
}