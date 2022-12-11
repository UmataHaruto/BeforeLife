#pragma comment(lib,"dxguid.lib")
#include	<array>
#include	"player.h"
#include	"CCamera.h"
#include    "skydome.h"
#include    "Effect.h"
#include    "DX11util.h"
#include    "mystring.h"
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
#include "Terrain.h"
#include <thread>
#include "BuildingMgr.h"
#include "RouteSearch.h"

Skydome g_skybox;       // スカイドーム
Terrain g_terrain;    //地形(外側)
XMFLOAT4X4 FirstCameraPos;

//時間計測
DWORD starttime;
DWORD endtime;
DWORD timer;

//試験的レンダリング
const DXGI_SURFACE_DESC* pDesc;
ID3D11Device* pDevice;
ID3D11Texture2D* mpTex;
ID3D11SamplerState* mpSmp;
ID3D11Buffer* mpQuadVB;		//デバッグ描画用頂点バッファ
ID3D11RenderTargetView* targetview;
ID3D11Resource*	g_TexResource;

ID3D11VertexShader* m_pVertexShader = nullptr;		// 頂点シェーダー入れ物
ID3D11PixelShader* m_pPixelShader = nullptr;		// ピクセルシェーダー入れ物
uint32_t mQuadStride;
ID3D11ShaderResourceView* SRV;

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
void  SimulationInit() {

	//ゲームボタン初期化
	GameButton::GetInstance().Init();

	// ASSIMPを使用したアニメーションの読み込み
	bool sts = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::PLAYER)].modelname)->LoadAnimation("assets/ModelData/male_adult/animation/male_adult_ver1.1.fbx");
	sts = ModelMgr::GetInstance().GetModelPtr(ModelMgr::GetInstance().g_modellist[static_cast<int>(MODELID::CONIFER00)].modelname)->LoadAnimation("assets/Modeldata/tree/conifer00/conifer01.fbx");
	
	//ステージをロード
	Stage::GetInstance().RoadStageData();


	//リソース初期化
	ResourceManager::GetInstance().Init();

	//建築物初期化
	BuildingMgr::GetInstance().Init();

	//ステージコリジョンを作成
	StageHitInit();

	//２Dの初期化
	Init2D();
	//２Dテキスト初期化
	MyString::InitMyString();

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

	//SetTime = 6:00
	Application::GAME_TIME = 360;

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

	//開始時間の設定
	starttime = timeGetTime();
}

void  SimulationExit() {


	VillagerMgr::GetInstance().Uninit();

	ResourceManager::GetInstance().Uninit();

	BuildingMgr::GetInstance().Uninit();

	MyString::ClearMyString();

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
	Stage::GetInstance().Update();

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
	float speed = 1;
	float dot = 0;


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
			FirstCameraPos._41 -= speed * FirstCameraPos._11;
			FirstCameraPos._43 -= speed * FirstCameraPos._13;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D)) {
			FirstCameraPos._41 += speed * FirstCameraPos._11;
			FirstCameraPos._43 += speed * FirstCameraPos._13;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			FirstCameraPos._41 += speed * FirstCameraPos._31;
			FirstCameraPos._43 += speed * FirstCameraPos._33;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S)) {
			FirstCameraPos._41 -= speed * FirstCameraPos._31;
			FirstCameraPos._43 -= speed * FirstCameraPos._33;
		}

		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT)) {
			CameraAngle.y-= 0.5;
		}
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT)) {
			CameraAngle.y += 0.5;
		}

		DX11MtxRotationY(CameraAngle.y, Rotation);
		RotationMtx = XMLoadFloat4x4(&Rotation);
		PositionMtx = XMLoadFloat4x4(&FirstCameraPos);
		PositionMtx = XMMatrixMultiply(RotationMtx, PositionMtx);
		XMStoreFloat4x4(&FirstCameraPos, PositionMtx);

		CCamera::GetInstance()->TPSCamera(FirstCameraPos);

		if (io.MouseWheel != 0)
		{
			if (io.MouseWheel > 0)
			{
				CCamera::GetInstance()->SetZoom(CCamera::GetInstance()->GetZoom() - 0.1);
			}
			else {
				CCamera::GetInstance()->SetZoom(CCamera::GetInstance()->GetZoom() + 0.1);
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

	Stage::GetInstance().Draw();
	//資源描画
	ResourceManager::GetInstance().Draw();

	//建築物描画
	BuildingMgr::GetInstance().Draw();
	GameButton::GetInstance().PrevieModelDraw();

	//村人描画
	VillagerMgr::GetInstance().Draw();

	g_skybox.Draw();

	g_terrain.Draw();
	//2D描画
	TurnOffZbuffer();

	Init2D();

	GameButton::GetInstance().Draw();

	Sprite2DMgr::GetInstance().Draw(CCamera::GetInstance()->GetCameraMatrix());
}
