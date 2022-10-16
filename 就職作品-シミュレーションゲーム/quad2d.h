#pragma once

#include	<d3d11.h>
#include	<DirectXMath.h>
#include	<wrl/client.h>
#include	<string>
#include	"dx11mathutil.h"
#include	"dx11util.h"
#include	"Shader.h"
#include	"shaderhashmap.h"
#include	"DX11Settransform.h"
#include	"uncopyable.h"

//エフェクトリスト
enum class UILIST
{
	BLACKBACK_START,
	BLACKBACK_END,
	ICON_MOUSE,
	_MAX,
};

enum class UISTATUS
{
	LIVE,
	DEAD
};

// 矩形クラス
class Quad2D{
public:
	// 頂点データ
	struct Vertex {
		DirectX::XMFLOAT3 pos;		// 座標		
		DirectX::XMFLOAT4 color;	// カラー
		DirectX::XMFLOAT2 tex;		// テクスチャ
	};

	ID3D11Buffer* m_vertexbuffer;			// 頂点バッファ

	// ビューポートのサイズをセット
	bool SetViewPortSize(uint32_t width,uint32_t height) {

		ID3D11Buffer* CBViewPort;

		// ビューポート用の定数バッファ
		struct ConstantBufferViewPort {
			uint32_t	Width[4];			// ビューポート幅
			uint32_t	Height[4];			// ビューポート高さ
		};

		// ビューポート定数バッファに値をセット
		bool sts = CreateConstantBufferWrite(
			GetDX11Device(), 
			sizeof(ConstantBufferViewPort), 
			&CBViewPort);
		if (!sts) {
			return false;
		}

		ConstantBufferViewPort cb;
		cb.Width[0] = width;
		cb.Height[0] = height;

		D3D11_MAPPED_SUBRESOURCE pData;
		HRESULT hr = GetDX11DeviceContext()->Map(CBViewPort, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
		if (SUCCEEDED(hr)) {
			memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(ConstantBufferViewPort));
			GetDX11DeviceContext()->Unmap(CBViewPort, 0);
		}
		else {
			return false;
		}

		// 頂点シェーダーにセット
		GetDX11DeviceContext()->VSSetConstantBuffers(5, 1,&CBViewPort);

		return true;
	}

	// 矩形の初期化
	bool Init(uint32_t width, uint32_t height, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2* uv);

	//更新処理
	void Update();

	// 描画
	void Draw();

	void UnInit();

	//ステータスを渡す
	UISTATUS GetStatus()
	{
		return m_sts;
	}

	//ポジションを渡す
	Vertex GetVertex()
	{
		return m_vertex[0];
	}

	XMFLOAT3 GetWorldPos()
	{
		XMFLOAT3 pos;
		pos.x = m_worldmtx._41;
		pos.y = m_worldmtx._42;
		pos.z = m_worldmtx._43;
		return pos;
	}

	//ステータスを設定
	void SetStatus(UISTATUS sts)
	{
		m_sts = sts;
	}

	void SetColor(XMFLOAT4 color)
	{
		m_color = color;
		for (int i = 0; i < 4; i++) {
			m_vertex[i].color = color;
		}
		updateVbuffer();
	}

	//ミニマップ座標を設定
	void SetMinimapPos(XMFLOAT3* pos)
	{
		m_minimapPos = pos;
	}

	XMFLOAT2 GetSize()
	{
		return XMFLOAT2(m_width,m_height);
	}

	//ミニマップ座標を取得
	XMFLOAT3* GetMinimapPos()
	{
		return m_minimapPos;
	}

	//アニメーション設定
	void SetAnimation(bool isAnimaiton, bool isLoop)
	{
		m_isAnimation = isAnimaiton;
		m_isLoop = isLoop;
	}

	//アニメーション遅延を設定
	void SetAnimationDelay(int Delay)
	{
		m_animationDelay = Delay;
		m_delaycount = 0;
	}

	//UVカット数をセット
	void SetCutUV(uint16_t CutU, uint16_t CutV)
	{
		m_cutU = CutU;
		m_cutV = CutV;
	}

	// 拡大、縮小
	void SetScale(float sx, float sy, float sz);

	// 位置をセット
	void SetPosition(float x, float y, float z);

	// Z軸回転
	void SetRotation(float angle);

	//UV設定
	void SetUV(DirectX::XMFLOAT2* uv);

	// 矩形の進行方向を取得（元画像が上向きに作られていることを前提）
	DirectX::XMFLOAT3 GetForward() {
		DirectX::XMFLOAT3 forward;
		forward.x = m_worldmtx._21;
		forward.y = m_worldmtx._22;
		forward.z = m_worldmtx._23;
		return forward;
	}

	// 矩形の横方向を取得
	DirectX::XMFLOAT3 GetSide() {
		DirectX::XMFLOAT3 side;
		side.x = m_worldmtx._11;
		side.y = m_worldmtx._12;
		side.z = m_worldmtx._13;
		return side;
	}

	UILIST GetType()
	{
		return type;
	}

	void SetType(UILIST ui)
	{
		type = ui;
	}

	void SetMtx(XMFLOAT4X4 InMtx)
	{
		m_worldmtx = InMtx;
	}

	// 頂点データ更新
	void updateVertex(uint32_t width, uint32_t height, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2* uv);
	// 頂点バッファ更新
	void updateVbuffer();

	// テクスチャセット
	void LoadTexture(const char* filename) {
		// テクスチャ読み込み
		bool sts = CreateSRVfromFile(filename,
			GetDX11Device(),
			GetDX11DeviceContext(),
			&m_tex, &m_srv);
		if (!sts) {
			std::string str("SRV 作成エラー ");
			std::string strfile(filename);
			MessageBox(nullptr, (str+strfile).c_str(), "error", MB_OK);
			return;
		}
	}

private:
	DirectX::XMFLOAT4X4		m_worldmtx;				// ワールド変換行列
	ID3D11Buffer*	m_indexbuffer;			// インデックスバッファ
	Quad2D::Vertex			m_vertex[4];			// 矩形４頂点
	float					m_width;				// 幅
	float					m_height;				// 高さ
	XMFLOAT4                m_color;           //色情報
	bool                    m_isAnimation;     //アニメーションか
	bool                    m_isLoop;          //アニメーションがループするか
	uint32_t                m_cutU;    //横の分割数
	uint32_t                m_cutV;    //縦の分割数
	int                     m_animationDelay;    //アニメーションの遅延度
	int                     m_delaycount;    //アニメーション遅延カウンター
	uint32_t                m_animationNo = 0;    //アニメーション番号
	UILIST                  type;   //UIタイプ
	UISTATUS                m_sts;    //Status
	XMFLOAT3*                m_minimapPos;    //ミニマップ上の座標
	ID3D11ShaderResourceView* m_srv;			// SRV
	ID3D11Resource* m_tex;					// テクスチャリソース
};