#pragma once

#include	<directxmath.h>
#include	<d3d11.h>
#include	"DX11util.h"
#include	"Shader.h"
#include    "CModel.h"
#include    "XAudio2.h"
/*----------------------------------------------------------------------

	ビルボードクラス	

-----------------------------------------------------------------------*/

//エフェクトリスト
enum class EFFECTLIST
{
	TARGETCIRCLE,
	PROGRESSBAR_CIRCLE,
	WOOD,
	ORE_COAL,
	ORE_IRON,
	ORE_GOLD,
	IRON,
	GOLD,
	HUKIDASHI_HEART,
	HUKIDASHI_ONPU,
	HUKIDASHI_ASE,
	HUKIDASHI_TALK,
	HUKIDASHI_QUESTION,
	_MAX,
};

enum class EFFECTSTATUS
{
	LIVE,
	DEAD
};

// 頂点フォーマット
struct MyVertex {
	float				x, y, z;
	DirectX::XMFLOAT4	color;
	float				tu, tv;
};

class CBillBoard{
	CModel* m_model;// ３Ｄモデル

	DirectX::XMFLOAT4X4			m_mtx;			// ビルボード用の行列
	float						m_x=0;			// ビルボードの位置座標
	float						m_y=0;					
	float						m_z=0;
	float						m_XSize=100.0f;		// ビルボードのＸサイズ
	float						m_YSize=100.0f;		// ビルボードのＹサイズ
	XMFLOAT3*                   m_Targetpos = nullptr;    //追尾先の移動量

	XMFLOAT3                    m_interval;  //ホーミング時の原点からの距離

	uint32_t                    m_life = 60; //HP
	uint32_t                    m_Maxlife = 90;
	EFFECTSTATUS                m_sts;    //ステータス
	EFFECTLIST                  m_type;

	bool                        m_isBillBolard;    //ビルボードを使用するか
	bool                        m_isAnimation;     //アニメーションか
	bool                        m_isLoop;          //アニメーションがループするか
	bool                        m_isHorming;
	int                     m_animationDelay;    //アニメーションの遅延度
	int                     m_delaycount;    //アニメーション遅延カウンター
	DirectX::XMFLOAT4			m_Color= DirectX::XMFLOAT4(1,1,1,1);	// 頂点カラー値
	ID3D11ShaderResourceView*   m_srv = nullptr;			// Shader Resourceviewテクスチャ
	ID3D11BlendState*			m_pBlendStateSrcAlpha = nullptr;
	ID3D11BlendState*			m_pBlendStateOne = nullptr;
	ID3D11BlendState*			m_pBlendStateDefault = nullptr;
	ID3D11VertexShader*			m_pVertexShader = nullptr;	// 頂点シェーダー入れ物
	ID3D11PixelShader*			m_pPixelShader = nullptr;	// ピクセルシェーダー入れ物
	ID3D11InputLayout*			m_pVertexLayout = nullptr;	// 頂点フォーマット定義
	ID3D11Resource*				m_res = nullptr;

	DirectX::XMFLOAT2		m_uv[4];				// テクスチャ座標

private:
	// ビルボードの頂点座標を計算
	void CalcVertex();
	// ビルボード用の行列を生成
	void CalcBillBoardMatrix(const DirectX::XMFLOAT4X4& cameramat);
	// 描画
	void Draw();
	// ソースアルファを設定する
	void SetBlendStateSrcAlpha();

	// ブレンドステートを生成する
	void CreateBlendStateSrcAlpha();
	// 加算合成を設定する
	void SetBlendStateOne();
	// ブレンドステート（加算合成）を生成する
	void CreateBlendStateOne();
	// デフォルトのブレンドステートを設定する
	void SetBlendStateDefault();
	// デフォルトのブレンドステートを生成する
	void CreateBlendStateDefault();

	//頂点バッファを削除
	void clearVBuffer()
	{

	}

public:
	uint32_t                    m_cutU;    //横の分割数
	uint32_t                    m_cutV;    //縦の分割数
	uint32_t                    m_animationNo = 0;    //アニメーション番号
	ID3D11Buffer* m_vbuffer = nullptr;		// 頂点バッファ
	MyVertex				m_Vertex[4];			// ビルボードの頂点座標

	//UV座標をセット
	void SetUV(DirectX::XMFLOAT2 uv[]) {
		m_uv[0] = uv[0];
		m_uv[1] = uv[1];
		m_uv[2] = uv[2];
		m_uv[3] = uv[3];
		CalcVertex();
	}
	//アニメーション設定
	void SetAnimation(bool isAnimaiton,bool isLoop)
	{
		m_isAnimation = isAnimaiton;
		m_isLoop = isLoop;
	}

	void SetAnimationNo(int AnimNo)
	{
		m_animationNo = AnimNo;
	}
	//モデルセット
	void SetModel(CModel* pmodel)
	{
		m_model = pmodel;
	}
	//ライフを設定する
	void SetLife(int hp)
	{
		m_life = hp;
	}
	//ステータスを設定
	void SetStatus(EFFECTSTATUS sts)
	{
		m_sts = sts;
	}
	//ビルボードとして扱うかを設定
	void SetBillBoard(bool trigger)
	{
		m_isBillBolard = trigger;
	}

	//ビルボードとして扱うかを設定
	void SetHorming(bool trigger)
	{
		m_isHorming = trigger;
	}

	//インターバルを設定
	void SetInterval(XMFLOAT3 pos)
	{
	     m_interval = pos;
	}

	void SetTargetP(XMFLOAT3* targetp)
	{
		if (m_Targetpos != targetp) {
			m_animationNo = 0;
			m_Targetpos = targetp;
		}
	}

	void DeleteTargetP(void)
	{
		m_Targetpos = nullptr;
		m_animationNo = 0;
	}

	void Uninit(void)
	{
		if (m_vbuffer != nullptr) {
			m_vbuffer->Release();
			m_vbuffer = nullptr;
		}
	}

	//ライフを渡す
	uint32_t GetLife()
	{
		return m_life;
	}
	//最大ライフを渡す
	uint32_t GetMaxLife()
	{
		return m_Maxlife;
	}
	EFFECTLIST GetType()
	{
		return m_type;
	}
	//ステータスを渡す
    EFFECTSTATUS GetStatus()
	{
	 return m_sts;
	}

	XMFLOAT3* GetTargetPos()
	{
		return m_Targetpos;
	}

	//ビルボードかどうかを渡す
	bool GetBillBoard()
	{
	    return m_isBillBolard;
	}
	CBillBoard():m_x(0),m_y(0),m_z(0),m_srv(nullptr){

	}

	bool Init(float x, float y, float z, float xsize, float ysize, DirectX::XMFLOAT4 color){
		m_x = x;
		m_y = y;
		m_z = z;
		m_XSize = xsize;
		m_YSize = ysize;
		m_Color = color;

		m_sts = EFFECTSTATUS::LIVE;

		// デバイス取得
		ID3D11Device* dev = GetDX11Device();
		// デバイスコンテキスト取得
		ID3D11DeviceContext* devcontext = GetDX11DeviceContext();

		// 頂点データの定義
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		unsigned int numElements = ARRAYSIZE(layout);

		// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
		bool sts = CreateVertexShader(dev,
			"shader/vsbillboard.hlsl",
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
			dev,							// デバイスオブジェクト
			"shader/psbillboard.hlsl", 
			"main", 
			"ps_4_0",
			&m_pPixelShader);

		if (!sts) {
			MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
			return false;
		}

		CalcVertex();						// ビルボード用の頂点データ作成	

		CreateBlendStateSrcAlpha();			// アルファブレンディング用ブレンドステート生成
		CreateBlendStateOne();				// 加算合成用のブレンドステート生成
		CreateBlendStateDefault();			// デフォルトのブレンドステート生成

		return true;
	}

	//更新処理
	void Update()
	{
		//フェード処理
		//XMFLOAT4 color = m_Color;
		//color.w = m_life / m_Maxlife;
		//SetColor(color);
		
		//アニメーションの場合
		m_delaycount++;

		static int SEdelay = 0;
		static bool TargetComplate = false;
		SEdelay++;
		//プログレスバーの場合
		if (m_delaycount >= m_animationDelay) {
			m_delaycount = 0;
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
				SetUV(uv);

				//ホーミングする場合
				if (m_isHorming && m_Targetpos != nullptr)
				{
					m_x = m_Targetpos->x + m_interval.x;
					m_y = m_Targetpos->y + m_interval.y;
					m_z = m_Targetpos->z + m_interval.z;
				}

				//アニメーションカウントを進める
				if (m_type != EFFECTLIST::PROGRESSBAR_CIRCLE) {
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
						m_sts = EFFECTSTATUS::DEAD;
					}
				}
			}
		}
	}

	void Dispose() {
		/*if (m_vbuffer != nullptr) {
			m_vbuffer->Release();
			m_vbuffer = nullptr;
		}*/

		//if (m_srv != nullptr) {
		//	m_srv->Release();
		//	m_srv = nullptr;
		//}

		//if (m_pBlendStateOne != nullptr) {
		//	m_pBlendStateOne->Release();
		//	m_pBlendStateOne = nullptr;
		//}

		//if (m_pBlendStateSrcAlpha != nullptr) {
		//	m_pBlendStateSrcAlpha->Release();
		//	m_pBlendStateSrcAlpha = nullptr;
		//}

		//if (m_pBlendStateDefault != nullptr) {
		//	m_pBlendStateDefault->Release();
		//	m_pBlendStateDefault = nullptr;
		//}

		//if (m_pPixelShader != nullptr) {
		//	m_pPixelShader->Release();
		//	m_pPixelShader = nullptr;
		//}

		//if (m_pVertexShader != nullptr) {
		//	m_pVertexShader->Release();
		//	m_pVertexShader = nullptr;
		//}

		//if (m_pVertexLayout != nullptr) {
		//	m_pVertexLayout->Release();
		//	m_pVertexLayout = nullptr;
		//}
	}

	// デストラクタ
	virtual ~CBillBoard(){
		Dispose();
	}

	// ＵＶ座標をセットする
	void SetUV(float u[],float v[]){
		m_Vertex[0].tu = u[0];
		m_Vertex[0].tv = v[0];

		m_Vertex[1].tu = u[1];
		m_Vertex[1].tv = v[1];

		m_Vertex[2].tu = u[2];
		m_Vertex[2].tv = v[2];

		m_Vertex[3].tu = u[3];
		m_Vertex[3].tv = v[3];

		CalcVertex();						// ビルボード用の頂点データ作成	
	}

	// 位置を指定
	void SetPosition(float x,float y,float z);

	// ビルボードを描画
	void DrawBillBoard(const DirectX::XMFLOAT4X4& cameramat);

	// ビルボード描画加算合成
	void DrawBillBoardAdd(const DirectX::XMFLOAT4X4& cameramat);

	// ビルボードをZ軸を中心にして回転させて描画
	void DrawRotateBillBoard(const DirectX::XMFLOAT4X4 &cameramat, float radian);
	
	// サイズをセット
	void SetSize(float x, float y);

	// カラーをセット
	void SetColor(DirectX::XMFLOAT4 col);

	//UVカット数をセット
	void SetCutUV(uint16_t CutU, uint16_t CutV)
	{
		m_cutU = CutU;
		m_cutV = CutV;
	}

	//アニメーション遅延を設定
	void SetAnimationDelay(int Delay)
	{
		m_animationDelay = Delay;
		m_delaycount = 0;
	}


	void SetType(EFFECTLIST type)
	{
		m_type = type;
	}

	//	テクスチャ読み込み
	bool LoadTexTure(const char* filename);

};