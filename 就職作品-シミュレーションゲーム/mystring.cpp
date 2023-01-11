#include	<vector>
#include	<DirectXMath.h>
#include	<wrl/client.h>
#include	"shaderhashmap.h"
#include	"mystring.h"
#include	"shader.h"
#include	"DX11Settransform.h"
#include	"dx11mathutil.h"
#include	"DX11util.h"

namespace MyString {

	using namespace DirectX;
	using  Microsoft::WRL::ComPtr;

	// 使用する頂点シェーダー名
	static const char* vsfilename[] = {
		"shader/basicvs.hlsl"
	};

	// 使用するピクセルシェーダー名
	static const char* psfilename[] = {
		"shader/basicps.hlsl",
	};

	// 頂点構造体
	struct Vertex {
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
		XMFLOAT2 Tex;
	};

	// インデックス構造体
	struct Face {
		unsigned int idx[3];
	};

	// 頂点データ
	static std::vector<Vertex> g_vertices;

	// インデックスデータ
	static std::vector<Face> g_indices;

	// 文字コードと表示するためのＵＶ座標
	struct mychar {
		char	code;			// 文字コード
		int x;					// 表示位置Ｘ
		int y;					// 表示位置Ｙ
		int	width;				// 幅
		int height;				// 高さ
		XMFLOAT4 color;			// 色
		XMFLOAT2 uv[4];			// UV座標
	};

	// 頂点バッファ、インデックスバッファ
	ComPtr<ID3D11Buffer>	g_VertexBuffer;
	ComPtr<ID3D11Buffer>	g_IndexBuffer;

	// ＡＳＣＩＩフォントリソース、ＳＲＶ
	ComPtr<ID3D11Resource>	g_TexResource;
	ComPtr<ID3D11ShaderResourceView>	g_SRV;

	// 表示文字列(最大１０２４文字)
	std::vector<mychar> g_mystring(MAXMOJICNT);

	// 頂点バッファ更新
	static void UpdateVertexBuffer() {
		D3D11_MAPPED_SUBRESOURCE pData;
		ID3D11DeviceContext* devcontext;
		devcontext = GetDX11DeviceContext();

		HRESULT hr = devcontext->Map(g_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
		if (SUCCEEDED(hr)) {
			memcpy_s(pData.pData, pData.RowPitch, (void*)(g_vertices.data()), sizeof(Vertex) * g_vertices.size());
			devcontext->Unmap(g_VertexBuffer.Get(), 0);
		}
	}

	// インデックスバッファ更新
	static void UpdateIndexBuffer() {
		D3D11_MAPPED_SUBRESOURCE pData;
		ID3D11DeviceContext* devcontext;
		devcontext = GetDX11DeviceContext();

		HRESULT hr = devcontext->Map(g_IndexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
		if (SUCCEEDED(hr)) {
			memcpy_s(pData.pData, pData.RowPitch, (void*)(g_indices.data()), sizeof(Face) * g_indices.size());
			devcontext->Unmap(g_IndexBuffer.Get(), 0);
		}
	}

	// 戻り値　FACE数
	int CreateVertexIndexDataFirst(int length) {
		int quadnum = 0;			// 四角形の数

		// 頂点初期化
		for (int idx = 0; idx < length; idx++) {
			Vertex v;
			v.Color = XMFLOAT4(0, 0, 0, 0);
			v.Pos.x = 0.0f;
			v.Pos.y = 0.0f;
			v.Pos.z = 0.0f;
			v.Tex.x = 0.0f;
			v.Tex.y = 0.0f;

			g_vertices.emplace_back(v);
			g_vertices.emplace_back(v);
			g_vertices.emplace_back(v);
			g_vertices.emplace_back(v);

			Face f;

			f.idx[0] = quadnum * 4;
			f.idx[1] = quadnum * 4 + 1;
			f.idx[2] = quadnum * 4 + 2;

			g_indices.emplace_back(f);

			f.idx[0] = quadnum * 4 + 1;
			f.idx[1] = quadnum * 4 + 3;
			f.idx[2] = quadnum * 4 + 2;

			g_indices.emplace_back(f);
		}

		return g_indices.size();
	}

	// 戻り値　FACE数
	int CreateVertexIndexData(int length) {
		int quadnum = 0;			// 四角形の数

		// 頂点初期化
		for (int idx = 0; idx < length; idx++) {
			Vertex v;

			v.Color = g_mystring[idx].color;
			v.Pos.x = static_cast<float>(g_mystring[idx].x);
			v.Pos.y = static_cast<float>(g_mystring[idx].y);
			v.Pos.z = 0.0f;
			v.Tex.x = g_mystring[idx].uv[0].x;
			v.Tex.y = g_mystring[idx].uv[0].y;
			g_vertices.push_back(v);

			v.Color = g_mystring[idx].color;
			v.Pos.x = static_cast<float>(g_mystring[idx].x + g_mystring[idx].width);
			v.Pos.y = static_cast<float>(g_mystring[idx].y);
			v.Pos.z = 0.0f;
			v.Tex.x = g_mystring[idx].uv[1].x;
			v.Tex.y = g_mystring[idx].uv[1].y;
			g_vertices.push_back(v);

			v.Color = g_mystring[idx].color;
			v.Pos.x = static_cast<float>(g_mystring[idx].x);
			v.Pos.y = static_cast<float>(g_mystring[idx].y + g_mystring[idx].height);
			v.Pos.z = 0.0f;
			v.Tex.x = g_mystring[idx].uv[2].x;
			v.Tex.y = g_mystring[idx].uv[2].y;
			g_vertices.push_back(v);

			v.Color = g_mystring[idx].color;
			v.Pos.x = static_cast<float>(g_mystring[idx].x + g_mystring[idx].width);
			v.Pos.y = static_cast<float>(g_mystring[idx].y + g_mystring[idx].height);
			v.Pos.z = 0.0f;
			v.Tex.x = g_mystring[idx].uv[3].x;
			v.Tex.y = g_mystring[idx].uv[3].y;
			g_vertices.push_back(v);

			Face f;

			f.idx[0] = quadnum * 4;
			f.idx[1] = quadnum * 4 + 1;
			f.idx[2] = quadnum * 4 + 2;

			g_indices.push_back(f);

			f.idx[0] = quadnum * 4 + 1;
			f.idx[1] = quadnum * 4 + 3;
			f.idx[2] = quadnum * 4 + 2;

			g_indices.push_back(f);

			quadnum++;
		}

		return g_indices.size();
	}

	// 初期化
	bool InitMyString() {

		int cnt = 0;

		// デバイスを取得
		ID3D11Device* device;
		device = GetDX11Device();

		// デバイスコンテキストを取得
		ID3D11DeviceContext* devicecontext;
		devicecontext = GetDX11DeviceContext();

		// 文字列サイズ初期化
		g_mystring.clear();

		// 頂点バッファとインデックスバッファの初期化データ生成
		CreateVertexIndexDataFirst(MAXMOJICNT);

		// 頂点バッファ生成
		bool sts = CreateVertexBufferWrite(device, sizeof(Vertex), MAXMOJICNT * 4, g_vertices.data(), &g_VertexBuffer);
		if (!sts) {
			MessageBox(nullptr, "CreateVerteBuffer error", "error", MB_OK);
			return false;
		}

		// インデックスバッファ生成
		sts = CreateIndexBufferWrite(device, MAXMOJICNT * 4 * 2, g_indices.data(), &g_IndexBuffer);
		if (!sts) {
			MessageBox(nullptr, "MyString CreateIndexBuffer error", "error", MB_OK);
			return false;
		}

		// ピクセルシェーダーを生成
		sts = ShaderHashmap::GetInstance()->SetPixelShader(psfilename[0]);
		if (!sts) {
			MessageBox(nullptr, "SetPixelShader error(basicps.hlsl)", "error", MB_OK);
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

		// SRV生成
		sts = CreateSRVfromFile("assets/FONT_A-OTF.png", device, devicecontext, &g_TexResource, &g_SRV);
		if (!sts) {
			MessageBox(nullptr, "CreateSRVfromFile error(sysfont.tga)", "error", MB_OK);
			return false;
		}

		// VECTOR初期化
		ClearMyString();

		return true;
	}

	// 文字列描画
	void DrawMyString() {

		CDirectXGraphics::GetInstance()->TurnOnAlphaBlending();

		XMFLOAT4X4 mtx;

		CreateVertexIndexData(static_cast<int>(g_mystring.size()));

		// 頂点バッファ更新
		UpdateVertexBuffer();

		// インデックスバッファ更新
		UpdateIndexBuffer();

		// デバイスコンテキストを取得
		ID3D11DeviceContext* devcontext;
		devcontext = GetDX11DeviceContext();

		// 座標変換用の行列をセット
		DX11MtxIdentity(mtx);
		DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, mtx);

		unsigned int stride = sizeof(Vertex);	// ストライドをセット（１頂点当たりのバイト数）
		unsigned  offset = 0;					// オフセット値をセット

		// 頂点バッファをデバイスコンテキストへセット
		devcontext->IASetVertexBuffers(
			0,									// スタートスロット
			1,									// 頂点バッファ個数
			g_VertexBuffer.GetAddressOf(),		// 頂点バッファの先頭アドレス
			&stride,							// ストライド
			&offset);							// オフセット

												// インデックスバッファをデバイスコンテキストへセット
		devcontext->IASetIndexBuffer(
			g_IndexBuffer.Get(),				// インデックスバッファ
			DXGI_FORMAT_R32_UINT,				// フォーマット
			0);									// オフセット

												// トポロジーをセット
		devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 頂点シェーダー、ピクセルシェーダー取得
		ID3D11VertexShader* vs;
		vs = ShaderHashmap::GetInstance()->GetVertexShader(vsfilename[0]);
		ID3D11PixelShader* ps;
		ps = ShaderHashmap::GetInstance()->GetPixelShader(psfilename[0]);

		// 頂点レイアウト取得
		ID3D11InputLayout* layout;

		layout = ShaderHashmap::GetInstance()->GetVertexLayout(vsfilename[0]);

		// 頂点シェーダーとピクセルシェーダーをセット
		devcontext->VSSetShader(vs, nullptr, 0);
		devcontext->PSSetShader(ps, nullptr, 0);

		// 頂点フォーマットをセット
		devcontext->IASetInputLayout(layout);

		// SRVをセット
		devcontext->PSSetShaderResources(0, 1, g_SRV.GetAddressOf());

		// ドローコール発行
		devcontext->DrawIndexed(
			g_indices.size()*3,		// インデックス数
			0,		// 開始インデックス
			0);		// 基準頂点インデックス

		ClearMyString();
	}

	// 描画する文字列をクリア
	void ClearMyString() {
		// 描画文字列をクリア
		g_mystring.clear();

		// インデックスクリア
		g_indices.clear();

		// 頂点クリア
		g_vertices.clear();
	}

	// アスキーフォント描画用のＵＶ座標取得
	void GetAsciiFontUV(char code, XMFLOAT2* uv) {

		int codex;
		int codey;

		codex = code & 0x0f;
		codey = code >> 4;

		codey = codey - 2;

		float tu, tv;

		tu = codex * 0.0625f;
		tv = codey * 0.0625f;

		uv[0].x = tu;
		uv[0].y = tv;

		uv[1].x = tu + 0.0625f;
		uv[1].y = tv;

		uv[2].x = tu;
		uv[2].y = tv + 0.0625f;

		uv[3].x = tu + 0.0625f;
		uv[3].y = tv + 0.0625f;
	}

	//　文字列描画
	void DrawString(int x, int y, int width, int height, XMFLOAT4 color, const char* str) {

		mychar	m;
		float distance = 5;

		//文字の縁を描画
		for (int midx = 0; midx < strlen(str); midx++) {
			m.code = str[midx];
			m.width = width + 4;
			m.height = height + 4;
			m.x = x + width * midx - (distance * midx) - 1;
			m.y = y-1;
			m.color = XMFLOAT4(0,0,0,0.8);

			// 文字コードからＡＳＣＩＩフォントのＵＶ座標を求める
			GetAsciiFontUV(m.code, m.uv);

			// 表示文字列情報を蓄える
			g_mystring.emplace_back(m);
		}

		//文字本体を描画
		for (int midx = 0; midx < strlen(str); midx++) {
			m.code = str[midx];
			m.width = width;
			m.height = height;
			m.x = x + width * midx - (distance * midx);
			m.y = y;
			m.color = color;
	
			// 文字コードからＡＳＣＩＩフォントのＵＶ座標を求める
			GetAsciiFontUV(m.code, m.uv);

			// 表示文字列情報を蓄える
			g_mystring.emplace_back(m);
		}
	}
}
