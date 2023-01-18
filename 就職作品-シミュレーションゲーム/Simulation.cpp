#pragma comment(lib,"dxguid.lib")
#include	<array>
#include	"player.h"
#include	"CCamera.h"
#include    "skydome.h"
#include    "Effect.h"
#include    "DX11util.h"
#include    "2dsystem.h"
#include    "Shader.h"
#include "DirectXMath.h"
#include "BoundingSphere.h"
#include "Enemy.h"
#include "ModelMgr.h"
#include "Application.h"
#include "CLight.h"
#include "CBillBoard.h"
#include "Stage.h"
#include "Sprite2DMgr.h"
#include "Button.h"
#include "myimgui.h"
#include <tchar.h>
#include	"WICTextureLoader.h"
#include "GameButton.h"
#include "mousepostoworld.h"
#include "VillagerMgr.h"
#include "NameGenerator.h"
#include "ResourceManager.h"
#include "XAudio2.h"
#include "StageHitCollider.h"
#include "Sea.h"
#include <thread>
#include "BuildingMgr.h"
#include "RouteSearch.h"
#include "shaderhashmap.h"
#include "DX11util.h"
#include "PerlinNoise.h"

Skydome g_skybox;       // スカイドーム
Sea g_terrain;    //地形(外側)
XMFLOAT4X4 FirstCameraPos;
//環境光バッファ
ID3D11Buffer* cb_Ambient;

//時間計測
DWORD starttime;
DWORD endtime;
DWORD timer;

void RenderDepthMap(CLight& light);
void SimulationUpdate();

// 頂点シェーダー
const char* vs_shadowmap_filename[] = {
	"shader/shadowmapvs.fx",
	"shader/vs_shadowmap.fx",
};

// ピクセルシェーダー
const char* ps_shadowmap_filename[] = {
	"shader/shadowmapps.fx",
	"shader/ps_shadowmap.fx",
};

const int DEPTHTEX_WIDTH = 4096 * 4;
const int DEPTHTEX_HEIGHT = 4096 * 4;

// シャドウマップ用定数バッファ
struct ConstantBufferShadowmap {
	DirectX::XMFLOAT4X4 ViewFromLight;				// 光源の位置カメラ
	DirectX::XMFLOAT4X4 ProjectionFromLight;		// 光源の位置カメラに対応したプロジェクション変換行列
	DirectX::XMFLOAT4X4 ScreenToUVCoord;			// スクリーン座標をテクスチャ座標空間に変換
};

// 頂点フォーマット定義
ComPtr<ID3D11InputLayout>			g_pVertexLayout;

ComPtr<ID3D11Texture2D>				g_DepthMapTex;		// テクスチャ
ComPtr<ID3D11ShaderResourceView>	g_DepthMapSRV;		// シェーダーリソースビュー
ComPtr<ID3D11RenderTargetView>		g_DepthMapRTV;		// レンダーターゲットビュー

ComPtr<ID3D11Texture2D>				g_DSTex;			// テクスチャ（DepthStencil）
ComPtr<ID3D11DepthStencilView>		g_DSTexDSV;			// ＤＳＶ

ComPtr<ID3D11Buffer>				g_ConstantBufferShadowmap;		// シャドウマップ用定数バッファ

enum class CAMERAMODE
{
	FIXED,
	DELAYTPS,
	TPS,
	FPS,
};

CAMERAMODE cameratype = CAMERAMODE::TPS;

//現在の移動数
int nowcnt;

//線形補間の回数
int lerpcnt = 20;

void SearchMousePosThread()
{
	XMFLOAT3 g_nearp = { 0,0,0 };
	XMFLOAT3 g_farp = { 0,0,0 };

	SearchMousePoint(g_nearp, g_farp);
}

void InitShadowMap()
{

	ID3D11Device* device;
	device = CDirectXGraphics::GetInstance()->GetDXDevice();


	// 頂点データの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	unsigned int numElements = ARRAYSIZE(layout);

	// ピクセルシェーダー生成
	for (int i = 0; i < 2; i++) {
		bool sts = ShaderHashmap::GetInstance()->SetPixelShader(ps_shadowmap_filename[i]);
		if (!sts) {
			std::string f(ps_shadowmap_filename[i]);
			std::string errormessage;
			errormessage = f + "SetPixelShader error";
			MessageBox(nullptr, errormessage.c_str(), "error", MB_OK);
		}
	}

	// 頂点シェーダー生成
	for (int i = 0; i < 2; i++) {
		bool sts = ShaderHashmap::GetInstance()->SetVertexShader(
			vs_shadowmap_filename[i],			// 頂点シェーダーファイル名
			layout,					// 頂点レイアウト
			numElements);			// エレメント数
		if (!sts) {
			std::string f(vs_shadowmap_filename[i]);
			std::string errormessage;
			errormessage = f + " " + "SetVertexShader error";
			MessageBox(nullptr, errormessage.c_str(), "error", MB_OK);
		}
	}

	//深度マップテクスチャーを作成
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = DEPTHTEX_WIDTH;
	desc.Height = DEPTHTEX_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.CPUAccessFlags = 0;	// CPUからはアクセスしない
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = 0;			// その他の設定なし
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法

	// シェーダ リソース ビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT; // フォーマット

	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;		// 2Dテクスチャ
	srDesc.Texture2D.MostDetailedMip = 0;					// 最初のミップマップ レベル
	srDesc.Texture2D.MipLevels = -1;						// すべてのミップマップ レベル

	// 2Dテクスチャを生成
	HRESULT hr = device->CreateTexture2D(
		&desc,							// 作成する2Dテクスチャの設定
		nullptr,						// 
		&g_DepthMapTex);				// 作成したテクスチャを受け取る変数
	if (FAILED(hr))
		MessageBox(nullptr, "CreateTexture error", "Error", MB_OK);

	// シェーダ リソース ビューの作成
	hr = device->CreateShaderResourceView(
		g_DepthMapTex.Get(),			// アクセスするテクスチャ リソース
		&srDesc,						// シェーダ リソース ビューの設定
		&g_DepthMapSRV);				// 受け取る変数
	if (FAILED(hr))
		MessageBox(nullptr, "SRV error", "Error", MB_OK);

	// レンダーターゲットビュー生成
	hr = device->CreateRenderTargetView(g_DepthMapTex.Get(),
		nullptr,
		&g_DepthMapRTV);
	if (FAILED(hr))
		MessageBox(nullptr, "RTV error", "Error", MB_OK);


	//デプスステンシルビュー用のテクスチャーを作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = DEPTHTEX_WIDTH;
	descDepth.Height = DEPTHTEX_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;

	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	hr = device->CreateTexture2D(&descDepth, nullptr, &g_DSTex);
	if (FAILED(hr)) {
		MessageBox(nullptr, "CreateTexture2D error", "error", MB_OK);
	}

	//そのテクスチャーに対しデプスステンシルビュー(DSV)を作成
	hr = device->CreateDepthStencilView(g_DSTex.Get(), nullptr, &g_DSTexDSV);
	if (FAILED(hr)) {
		MessageBox(nullptr, "CreateDepthStencilView error", "error", MB_OK);
	}

	// 定数バッファ生成
	bool sts = CreateConstantBufferWrite(
		device,
		sizeof(ConstantBufferShadowmap),
		&g_ConstantBufferShadowmap);			// シャドウマップ用定数バッファ
}

void  SimulationInit() {
	// ASSIMPを使用したアニメーションの読み込み
	bool sts = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname)->LoadAnimation("assets/ModelData/male_adult/animation/male_adult_ver1.1.fbx");
	sts = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CONIFER00)].modelname)->LoadAnimation("assets/Modeldata/tree/conifer00/conifer01.fbx");
	
	//ステージをロード
	Stage::GetInstance().RoadStageData();
	Stage::GetInstance().Init();

	GameButton::GetInstance().Init();

	//リソース初期化
	ResourceManager::GetInstance().Init();

	//建築物初期化
	BuildingMgr::GetInstance().Init();

	//ステージコリジョンを作成
	StageHitInit();

	//地形生成パーリンノイズ
	Resource::Data resource_data;
	//スポーン座標
	int endurance = 100;
	int endurancemax = 100;
	int hardness = 0;
	int amount = 1;

	int model_select = 0;
	int modeltype_select = 1;

	PerlinNoise noise;
	std::array < PerlinNoise::Pint, 40000 > noise_array;
	noise_array = noise.setSeed(1);

	for (int j = 0; j < 200; j++)
	{
	    for (int i = 0; i < 200; i++)
	    {
	        if (noise_array[i + (j * 200)] > 30 && noise_array[i + (j * 200)] < 32) {
				resource_data.type = ItemType::WOOD;
				resource_data.pos = XMFLOAT3(i * 12.5,0,-j * 12.5);
				resource_data.Endurance = endurance;
				resource_data.EnduranceMax = endurancemax;
				resource_data.Hardness = hardness;
				resource_data.amount = amount;
				ResourceManager::GetInstance().CreateResource(resource_data,MODELID::CONIFER00);
			}
			if (noise_array[i + (j * 200)] > 40 && noise_array[i + (j * 200)] < 42) {
				resource_data.type = ItemType::ORE_IRON;
				resource_data.pos = XMFLOAT3(i * 12.5, 0, -j * 12.5);
				resource_data.Endurance = endurance;
				resource_data.EnduranceMax = endurancemax;
				resource_data.Hardness = hardness;
				resource_data.amount = amount;
				ResourceManager::GetInstance().CreateResource(resource_data, MODELID::ORE_IRON);
			}
	    }
	}

	//ノイズから資源を生成


	//２Dの初期化
	Init2D();

	InitShadowMap();

	//初期建造物
    //住居
	{
		House::Data initdata;
		initdata.pos = XMFLOAT3(500, 0, -430);
		initdata.type = GameButton::HouseButtonType::HOUSE_SMALL;
		BuildingMgr::GetInstance().CreateHouse(initdata, MODELID::SMALLHOUSE);
	}
	//倉庫
	{
		Souko::Data initdata;
		initdata.pos = XMFLOAT3(650, 0, -400);
		initdata.type = GameButton::SoukoButtonType::SOUKO_SMALL;
		initdata.store_max = 20;
		BuildingMgr::GetInstance().CreateSouko(initdata, MODELID::SMALLSOUKO);
	}

	//村人マネージャーの初期化
	VillagerMgr::GetInstance().Init();
	Player::Data pdata;
	pdata.pos = XMFLOAT3(500, 0, -500);
	pdata.firstname = NameGenerator::GetInstance().CreateName(NameGenerator::MALE);
	pdata.lastname = NameGenerator::GetInstance().CreateName(NameGenerator::FAMILLY);
	pdata.hp_max = 100;
	pdata.hp = 80;
	pdata.mood = 70;
	pdata.stamina_max = 100;
	pdata.stamina = 85;
	VillagerMgr::GetInstance().CreateVillager(pdata);

	//カメラ初期位置の初期化
	DX11MtxIdentity(FirstCameraPos);
	FirstCameraPos._41 = VillagerMgr::GetInstance().m_villager[0]->GetPos().x;
	FirstCameraPos._42 = VillagerMgr::GetInstance().m_villager[0]->GetPos().y;
	FirstCameraPos._43 = VillagerMgr::GetInstance().m_villager[0]->GetPos().z;

	CCamera::GetInstance()->SetEye(VillagerMgr::GetInstance().m_villager[0]->GetPos());
	CCamera::GetInstance()->CreateCameraMatrix();

	//SetTime = 11:00
	Application::GAME_TIME = 660;

	Sprite2DMgr::GetInstance().CreateUI(UILIST::BLACKBACK_START, 1000, 530, 0, 11, 5.5, XMFLOAT4(1, 1, 1, 1));

	//MouseCursor生成
	Sprite2DMgr::GetInstance().CreateUI(UILIST::ICON_MOUSE, 1000, 530, 0, 0.2,0.2, XMFLOAT4(1, 1, 1, 1));

	// ライト初期化
	XMFLOAT4 lightpos(0, 0, 1, 0);

	XMFLOAT2 uv[4];
	uv[0].x = 0.0f;
	uv[0].y = 0.0f;
	uv[1].x = 1.0f;
	uv[1].y = 0.0f;
	uv[2].x = 0.0f;
	uv[2].y = 1.0f;
	uv[3].x = 1.0f;
	uv[3].y = 1.0f;

	//スカイドーム初期化
	g_skybox.Init();
	g_skybox.SetModel(ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::SKYBOX)].modelname));

	//地形初期化
	g_terrain.Init();

	//環境光バッファ
	ID3D11Device* dev;
	dev = CDirectXGraphics::GetInstance()->GetDXDevice();

	// コンスタントバッファ作成
	sts = CreateConstantBuffer(
		dev,				// デバイス
		sizeof(XMFLOAT4),		// サイズ
		&cb_Ambient);			// コンスタントバッファ7
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(constant buffer Light) error", "Error", MB_OK);
	}

	//開始時間の設定
	starttime = timeGetTime();

	SoundMgr::GetInstance().XA_Play("assets/sound/BGM/Vopna.wav");

	SimulationUpdate();

	//ステージ更新
	Stage::GetInstance().Update();
}

void  SimulationExit() {


	VillagerMgr::GetInstance().Uninit();

	ResourceManager::GetInstance().Uninit();

	BuildingMgr::GetInstance().Uninit();

	//スカイドーム終了処理
	g_skybox.Finalize();

	g_terrain.Finalize();

	imguiExit();

}

void  SimulationUpdate() {

	ImGuiIO& io = ImGui::GetIO();

	//経過時間を設定(ミリ秒)
	endtime = timeGetTime();
	timer = (endtime - starttime);

	CDirectInput::GetInstance().GetMouseState();

	//資源マネージャーの更新
	ResourceManager::GetInstance().Update();

	//建築物マネージャーの更新
	BuildingMgr::GetInstance().Update();


	//村人マネージャーの更新
	VillagerMgr::GetInstance().Update();

	//ステージ更新
	//Stage::GetInstance().Update();

	//ブロック選択
	//Stage::GetInstance().SearchBlock();

	//スプライトマネージャーの更新
	Sprite2DMgr::GetInstance().Update();

	//ボタン更新
	GameButton::GetInstance().Update();

	//F5キー入力(トリガー)
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_F5))
	{
		switch (cameratype)
		{
		case CAMERAMODE::FIXED:
			cameratype = CAMERAMODE::DELAYTPS;

			break;

		case CAMERAMODE::DELAYTPS:
			cameratype = CAMERAMODE::TPS;
			break;

		case CAMERAMODE::TPS:
			cameratype = CAMERAMODE::FPS;
			break;

		case CAMERAMODE::FPS:
			cameratype = CAMERAMODE::FIXED;
			break;

		default:
			break;
		}
	}

	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_F8)) {
		RouteSearch::GetInstance().InitStageCollider();
		RouteSearch::GetInstance().SearchRoute(XMFLOAT3(300,0,300), XMFLOAT3(20, 0, 20));
	}
	//スカイドーム更新
	g_skybox.Update(XMFLOAT3(0,0,0));
	XMFLOAT4X4 Rotation;
	XMMATRIX RotationMtx;
	XMMATRIX PositionMtx;
	XMFLOAT3 CameraAngle = {0,0,0};
	XMFLOAT3 Position = CCamera::GetInstance()->GetEye();
	XMFLOAT4X4 CameraMtx = CCamera::GetInstance()->GetCameraMatrix();
	XMFLOAT3 FocusPos = CCamera::GetInstance()->GetLookat();
	float speed = 5;
	float dot = 0;

	XMFLOAT3 MaxEyePos = XMFLOAT3(2500, 1000,200);
	XMFLOAT3 MinEyePos = XMFLOAT3(0, -50, -2500);

	XMFLOAT4X4 CameraPos_old = FirstCameraPos;
	DX11MtxIdentity(Rotation);
	//カメラタイプに応じて設定
	switch (cameratype)
	{
	case CAMERAMODE::FIXED:
		CCamera::GetInstance()->FixedCamera(XMFLOAT3(0,0,0));
		break;

	case CAMERAMODE::DELAYTPS:
		CCamera::GetInstance()->DelayTPSCamera(VillagerMgr::GetInstance().m_villager[0]->GetMtx());
		break;

	case CAMERAMODE::TPS:
		//ズーム倍率 基準1.0f
		float zoom;
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
			Stage::GetInstance().Update();
				CameraPos_old._41 -= speed * FirstCameraPos._11;
				CameraPos_old._43 -= speed * FirstCameraPos._13;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D)) {
			Stage::GetInstance().Update();
				CameraPos_old._41 += speed * FirstCameraPos._11;
				CameraPos_old._43 += speed * FirstCameraPos._13;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			Stage::GetInstance().Update();
				CameraPos_old._41 += speed * FirstCameraPos._31;
				CameraPos_old._43 += speed * FirstCameraPos._33;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S)) {
				Stage::GetInstance().Update();
				CameraPos_old._41 -= speed * FirstCameraPos._31;
				CameraPos_old._43 -= speed * FirstCameraPos._33;
		}
		if (CameraPos_old._41 < MaxEyePos.x && CameraPos_old._42 < MaxEyePos.y && CameraPos_old._43 < MaxEyePos.z && CameraPos_old._41 > MinEyePos.x && CameraPos_old._42 > MinEyePos.y && CameraPos_old._43 > MinEyePos.z)
		{
			FirstCameraPos = CameraPos_old;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT)) {
			Stage::GetInstance().Update();
			CameraAngle.y-= 0.5;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT)) {
			Stage::GetInstance().Update();
			CameraAngle.y += 0.5;
		}
		if (FirstCameraPos._41 < MaxEyePos.x && FirstCameraPos._42 < MaxEyePos.y && FirstCameraPos._43 < MaxEyePos.z && FirstCameraPos._41 > MinEyePos.x && FirstCameraPos._42 > MinEyePos.y && FirstCameraPos._43 > MinEyePos.z)
		{
			DX11MtxRotationY(CameraAngle.y, Rotation);
			RotationMtx = XMLoadFloat4x4(&Rotation);
			PositionMtx = XMLoadFloat4x4(&FirstCameraPos);
			PositionMtx = XMMatrixMultiply(RotationMtx, PositionMtx);
			XMStoreFloat4x4(&FirstCameraPos, PositionMtx);
			CCamera::GetInstance()->SetEye(XMFLOAT3(FirstCameraPos._41, FirstCameraPos._42, FirstCameraPos._43));

			CCamera::GetInstance()->TPSCamera(FirstCameraPos);
			if (io.MouseWheel != 0)
			{
				if (io.MouseWheel > 0)
				{
					Stage::GetInstance().Update();
					CCamera::GetInstance()->SetZoom(CCamera::GetInstance()->GetZoom() - 0.1);
				}
				else {
					Stage::GetInstance().Update();
					CCamera::GetInstance()->SetZoom(CCamera::GetInstance()->GetZoom() + 0.1);
				}
			}
		}
		break;

	default:

		break;
	}
}

void  SimulationDraw() {
	//３D描画
	TurnOnZbuffer();
	CLight* light = GetDirectionallight();
	RenderDepthMap(*light);

	// デバイスコンテキスト取得
	ID3D11DeviceContext* devcontext;
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	// レンダリングターゲットビュー、デプスステンシルビューを設定
	ID3D11RenderTargetView* rtv[] = { CDirectXGraphics::GetInstance()->GetRenderTargetView() };
	devcontext->OMSetRenderTargets(1, rtv, CDirectXGraphics::GetInstance()->GetDepthStencilView());

	// ビューポートを設定
	D3D11_VIEWPORT vp;

	vp.Width = static_cast<float>(CDirectXGraphics::GetInstance()->GetViewPortWidth());
	vp.Height = static_cast<float>(CDirectXGraphics::GetInstance()->GetViewPortHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	devcontext->RSSetViewports(1, &vp);

	// 定数バッファを６番スロットにセット
	devcontext->PSSetConstantBuffers(6, 1, g_ConstantBufferShadowmap.GetAddressOf());
	devcontext->VSSetConstantBuffers(6, 1, g_ConstantBufferShadowmap.GetAddressOf());

	// depthmapをセット
	devcontext->PSSetShaderResources(1, 1, g_DepthMapSRV.GetAddressOf());

	ID3D11InputLayout* layout;
	layout = ShaderHashmap::GetInstance()->GetVertexLayout(vs_shadowmap_filename[1]);
	ID3D11VertexShader* vsh;
	vsh = ShaderHashmap::GetInstance()->GetVertexShader(vs_shadowmap_filename[1]);
	ID3D11PixelShader* psh;
	psh = ShaderHashmap::GetInstance()->GetPixelShader(ps_shadowmap_filename[1]);

	g_terrain.Draw();

	Stage::GetInstance().Draw();

	//資源描画
	ResourceManager::GetInstance().Draw();

	//建築物描画
	BuildingMgr::GetInstance().Draw();
	GameButton::GetInstance().PrevieModelDraw();

	//村人描画
	VillagerMgr::GetInstance().Draw();


	//g_skybox.Draw();

	//2D描画
	TurnOffZbuffer();

	Init2D();

	GameButton::GetInstance().Draw();

	Sprite2DMgr::GetInstance().Draw(CCamera::GetInstance()->GetCameraMatrix());
}

// 深度バッファを描画
void RenderDepthMap(CLight& light) {
	// スクリーン座標をＵＶ座標に変換する行列
	ALIGN16 XMMATRIX g_uvmatrix = XMMATRIX(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	// ターゲットバッファクリア	
	float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //red,green,blue,alpha

	// デバイスコンテキスト取得
	ID3D11DeviceContext* devcontext;
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	// レンダリングターゲットビュー、デプスステンシルビューを設定
	ID3D11RenderTargetView* rtv[] = { g_DepthMapRTV.Get() };
	devcontext->OMSetRenderTargets(1, rtv, g_DSTexDSV.Get());

	// ビューポートを設定
	D3D11_VIEWPORT vp;
	vp.Width = DEPTHTEX_WIDTH;
	vp.Height = DEPTHTEX_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	devcontext->RSSetViewports(1, &vp);

	// ターゲットバッファクリア
	devcontext->ClearRenderTargetView(g_DepthMapRTV.Get(), ClearColor);
	// Zバッファクリア
	devcontext->ClearDepthStencilView(g_DSTexDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 深度バッファを生成する為のシェーダー情報を取得
	ID3D11InputLayout* layout;
	layout = ShaderHashmap::GetInstance()->GetVertexLayout(vs_shadowmap_filename[0]);
	ID3D11VertexShader* vsh;
	vsh = ShaderHashmap::GetInstance()->GetVertexShader(vs_shadowmap_filename[0]);
	ID3D11PixelShader* psh;
	psh = ShaderHashmap::GetInstance()->GetPixelShader(ps_shadowmap_filename[0]);

	// 頂点フォーマットをセット
	devcontext->IASetInputLayout(layout);
	// 頂点シェーダーをセット
	devcontext->VSSetShader(vsh, nullptr, 0);
	// ピクセルシェーダーをセット
	devcontext->PSSetShader(psh, nullptr, 0);

	// カメラの位置
	XMFLOAT3 LightPos;
	float degree = 30;

	//深夜(00:00～)
	if (Application::GAME_TIME > 0 && Application::GAME_TIME < 420)
	{
		degree = 85;
		degree += (Application::GAME_TIME * 0.13);

	}

	//日の出(07:00～)時の角度
	//時間による光源移動
	if (Application::GAME_TIME > 420 && Application::GAME_TIME < 1020) {
		degree += ((Application::GAME_TIME - 420) * 0.2);
	}
	//夜(19:00～)
	if (Application::GAME_TIME > 1020 && Application::GAME_TIME <= 1440)
	{
		degree = 30;
		degree += ((Application::GAME_TIME - 1020) * 0.2);

	}

	LightPos.x = (2000 * cos(degree * XM_PI / 180.0f)) + 700;
	LightPos.z = -700;
	LightPos.y = 3500 * sin(degree * XM_PI / 180.0f);


	XMFLOAT4 l;
	l.x = LightPos.x;
	l.y = LightPos.y;
	l.z = LightPos.z;
	l.w = 1.0f;
	light.SetLightPos(l);
	light.Update();

	//環境光（color)
	XMFLOAT4 cb = XMFLOAT4(0,0,0,0);
	//深夜(0:00～05:00)
	if (Application::GAME_TIME >= 0 && Application::GAME_TIME <= 300)
	{
		cb.x = 58;
		cb.y = 64;
		cb.z = 124;
		cb.w = 255;
	}
	//日の出(5:01～7:00)
	if (Application::GAME_TIME > 300 && Application::GAME_TIME <= 420)
	{
		cb.x = 58 + (((float)(61 - 58) / 120) * (Application::GAME_TIME - 300));
		cb.y = 64 + (((float)(81 - 64) / 120) * (Application::GAME_TIME - 300));
		cb.z = 124 - (((float)(124 - 87) / 120) * (Application::GAME_TIME - 300));
		cb.w = 255;
	}

	//日の出(7:01～13:00)
	if (Application::GAME_TIME > 420 && Application::GAME_TIME <= 780)
	{
		cb.x = 61 + (((float)(255 - 61) / 360) * (Application::GAME_TIME - 420));
		cb.y = 81 + (((float)(255 - 81) / 360) * (Application::GAME_TIME - 420));
		cb.z = 87 + (((float)(255 - 87) / 360) * (Application::GAME_TIME - 420));
		cb.w = 255;
	}
	//夕暮れ(13:01～21:00)
	if (Application::GAME_TIME > 780 && Application::GAME_TIME <= 1260)
	{
		cb.x = 255 - (((float)(255 - 58) / 500) * (Application::GAME_TIME - 780));
		cb.y = 255 - (((float)(255 - 64) / 500) * (Application::GAME_TIME - 780));
		cb.z = 255 - (((float)(255 - 124) / 500) * (Application::GAME_TIME - 780));
		cb.w = 255;
	}
	//夜間(21:01～0:00)
	if (Application::GAME_TIME > 1260 && Application::GAME_TIME <= 1440)
	{
		cb.x = 58;
		cb.y = 64;
		cb.z = 124;
		cb.w = 255;
	}
	//0-1の間に補間
	cb.x /= 255;
	cb.y /= 255;
	cb.z /= 255;
	cb.w /= 255;

	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	devcontext->UpdateSubresource(cb_Ambient,
		0,
		nullptr,
		&cb,
		0, 0);

	// コンスタントバッファ4をｂ3レジスタへセット（頂点シェーダー用）
	devcontext->VSSetConstantBuffers(7, 1, &cb_Ambient);
	// コンスタントバッファ4をｂ3レジスタへセット(ピクセルシェーダー用)
	devcontext->PSSetConstantBuffers(7, 1, &cb_Ambient);

	ALIGN16 XMVECTOR Eye = XMVectorSet(LightPos.x, LightPos.y, LightPos.z, 0.0f);
	ALIGN16 XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	ALIGN16 XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	ALIGN16 XMMATRIX camera;
	camera = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));

	// ビュー変換行列をセット（光源位置からのカメラ）
	ConstantBufferShadowmap cb_shadow;
	XMStoreFloat4x4(&cb_shadow.ViewFromLight, camera);

	// 光源カメラ用のプロジェクション変換行列
	float	nearclip = GameButton::GetInstance().GetShadowParameter().nearclip;
	float	farclip = GameButton::GetInstance().GetShadowParameter().farclip;
	float	Aspect = GameButton::GetInstance().GetShadowParameter().Aspect;
	float	Fov = GameButton::GetInstance().GetShadowParameter().Fov;

	ALIGN16 XMMATRIX ProjectionFromLight;

	ProjectionFromLight = XMMatrixTranspose(XMMatrixPerspectiveFovLH(Fov, Aspect, nearclip, farclip));
	XMStoreFloat4x4(&cb_shadow.ProjectionFromLight, ProjectionFromLight);		// プロジェクション変換行列をセット

	// スクリーン座標をＵＶ座標に変換する行列
	XMStoreFloat4x4(&cb_shadow.ScreenToUVCoord, XMMatrixTranspose(g_uvmatrix));

	// 定数バッファに反映させる
	D3D11_MAPPED_SUBRESOURCE pData;

	HRESULT hr = devcontext->Map(g_ConstantBufferShadowmap.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&pData);
	if (SUCCEEDED(hr)) {
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb_shadow), sizeof(ConstantBufferShadowmap));
		devcontext->Unmap(g_ConstantBufferShadowmap.Get(), 0);
	}

	// 定数バッファを６番スロットにセット
	devcontext->PSSetConstantBuffers(6, 1, g_ConstantBufferShadowmap.GetAddressOf());
	devcontext->VSSetConstantBuffers(6, 1, g_ConstantBufferShadowmap.GetAddressOf());

	//資源描画
	ResourceManager::GetInstance().DrawShadow(layout,vsh,psh);

	//建築物描画
	BuildingMgr::GetInstance().DrawShadow(layout, vsh, psh);

	//村人描画
	VillagerMgr::GetInstance().DrawShadow(layout, vsh, psh);
}